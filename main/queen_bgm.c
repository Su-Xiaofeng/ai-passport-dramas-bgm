/* 重生之我是女王 BGM. One worker exclusively owns the codec/model; buttons enqueue lightweight
 * events and the UI consumes copied snapshots. Both workers acknowledge stop
 * before their queues or screen can be released. */
#include "queen_bgm.h"
#include "bgm_adpcm.h"
#include "bgm_model.h"
#include "bgm_tracks.h"
#include "bgm_ui.h"
#include "bsp_audio.h"
#include "bsp_battery.h"
#include "bsp_button.h"
#include "bsp_display.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <string.h>

static const char *TAG = "queen_bgm";
static QueueHandle_t s_commands, s_snapshots;
static bool s_audio_ready, s_battery_ready, s_ui_ready;
static TaskHandle_t s_audio_task, s_ui_task;
static SemaphoreHandle_t s_audio_stopped, s_ui_stopped;
static volatile bool s_stop_requested;
static bool s_audio_acked, s_ui_acked;
static esp_err_t s_stop_result;
typedef struct { bgm_model_t model; int64_t last_input_us; } snapshot_t;

void queen_bgm_key(bsp_btn_t btn, bsp_btn_ev_t event) {
    if (s_stop_requested) return;
    bgm_action_t action;
    if (event == BSP_BTN_CLICK) {
        action = btn == BSP_BTN_UP ? BGM_PREVIOUS : btn == BSP_BTN_DOWN ? BGM_NEXT : BGM_TOGGLE;
    } else if (event == BSP_BTN_LONG) {
        action = btn == BSP_BTN_UP ? BGM_VOLUME_UP : btn == BSP_BTN_DOWN ? BGM_VOLUME_DOWN : BGM_REPEAT;
    } else return;
    if (s_commands) (void)xQueueSend(s_commands, &action, 0);
}

static void publish(const bgm_model_t *model, int64_t last_input) {
    const snapshot_t snapshot = { *model, last_input };
    (void)xQueueOverwrite(s_snapshots, &snapshot);
}

static void audio_task(void *arg) {
    (void)arg;
    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    bgm_model_t model;
    bgm_model_init(&model);
    bgm_adpcm_t decoder;
    bgm_adpcm_reset(&decoder);
    bool available[BGM_TRACK_COUNT];
    for (size_t i = 0; i < BGM_TRACK_COUNT; i++) {
        available[i] = bgm_tracks[i].data && bgm_tracks[i].samples &&
                       bgm_tracks[i].bytes >= (bgm_tracks[i].samples + 1u) / 2u;
    }
    if (!s_audio_ready) model.error = true;
    bool awake = false;
    uint32_t generation = model.generation;
    int64_t last_input = esp_timer_get_time(), last_publish = 0, last_feed = 0, max_feed_gap = 0;
    /* Only this worker accesses PCM. Keep the buffer off its stack so codec
     * wake/open and formatted driver logs have room for their call frames. */
    static int16_t pcm[512];
    publish(&model, last_input);
    if (s_audio_ready && bsp_audio_sleep() != ESP_OK) {
        bgm_model_apply(&model, BGM_FAILED, available);
        publish(&model, last_input);
    }
    while (!s_stop_requested) {
        bgm_action_t action;
        if (xQueueReceive(s_commands, &action, model.playing ? 0 : pdMS_TO_TICKS(100)) == pdTRUE) {
            last_input = esp_timer_get_time();
            bgm_model_apply(&model, action, available);
            if (model.playing && !s_audio_ready) bgm_model_apply(&model, BGM_FAILED, available);
            if (awake && (action == BGM_VOLUME_UP || action == BGM_VOLUME_DOWN)) bsp_audio_set_volume(model.volume);
            publish(&model, last_input);
        }
        if (generation != model.generation) {
            /* Drop queued old-track PCM before reconfiguration; no other task
             * can write PCM or control codec sleep/format/volume. */
            if (awake) {
                esp_err_t err = bsp_audio_sleep();
                awake = false;
                if (err != ESP_OK) bgm_model_apply(&model, BGM_FAILED, available);
            }
            bgm_adpcm_reset(&decoder);
            generation = model.generation;
            last_feed = max_feed_gap = 0;
        }
        if (!model.playing) {
            if (awake) {
                esp_err_t err = bsp_audio_sleep();
                awake = false;
                if (err != ESP_OK) bgm_model_apply(&model, BGM_FAILED, available);
                publish(&model, last_input);
            }
            continue;
        }
        if (!awake) {
            esp_err_t err = bsp_audio_wake();
            if (err == ESP_OK) err = bsp_audio_set_format(BGM_SAMPLE_RATE, 16, 1);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Cannot start codec: %s", esp_err_to_name(err));
                bgm_model_apply(&model, BGM_FAILED, available);
                publish(&model, last_input);
                continue;
            }
            bsp_audio_set_volume(model.volume);
            awake = true;
            ESP_LOGI(TAG, "Playback ready track=%u; audio stack minimum free=%u",
                     model.track + 1, (unsigned)uxTaskGetStackHighWaterMark(NULL));
            last_feed = 0;
        }
        const bgm_track_t *track = &bgm_tracks[model.track];
        size_t n = bgm_adpcm_decode(&decoder, track->data, track->bytes, track->samples, pcm, 512);
        if (!n || bsp_audio_write(pcm, n * sizeof(*pcm)) != ESP_OK) {
            ESP_LOGE(TAG, "Decode or PCM write failed for track %u", model.track + 1);
            bgm_model_apply(&model, BGM_FAILED, available);
            publish(&model, last_input);
            continue;
        }
        int64_t now = esp_timer_get_time();
        if (last_feed && now - last_feed > max_feed_gap) max_feed_gap = now - last_feed;
        last_feed = now;
        model.position = decoder.sample;
        if (decoder.sample == track->samples) {
            /* Feed enough silence to let the last queued PCM reach the DAC. */
            memset(pcm, 0, sizeof(pcm));
            bool flush_ok = true;
            for (int i = 0; i < 3; i++) {
                if (bsp_audio_write(pcm, sizeof(pcm)) != ESP_OK) flush_ok = false;
            }
            ESP_LOGI(TAG, "Track %u finished; max feed interval=%lld us", model.track + 1, (long long)max_feed_gap);
            bgm_model_apply(&model, flush_ok ? BGM_FINISHED : BGM_FAILED, available);
        }
        if (now - last_publish >= 100000 || !model.playing || !model.position) {
            publish(&model, last_input);
            last_publish = now;
        }
        vTaskDelay(1); /* Also yield while the DMA queue still has spare space. */
    }
    s_stop_result = s_audio_ready ? bsp_audio_sleep() : ESP_OK;
    xSemaphoreGive(s_audio_stopped);
    vTaskSuspend(NULL); // Owner deletes this task only after acknowledgement.
}

static void ui_task(void *arg) {
    (void)arg;
    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    snapshot_t snapshot = {0};
    int battery = -1;
    int64_t battery_due = 0;
    bool dimmed = false;
    while (!s_stop_requested) {
        bool changed = xQueueReceive(s_snapshots, &snapshot, pdMS_TO_TICKS(100)) == pdTRUE;
        int64_t now = esp_timer_get_time();
        if (now >= battery_due) {
            battery = s_battery_ready ? bsp_battery_soc() : -1;
            battery_due = now + 10000000;
            changed = true;
        }
        bool should_dim = now - snapshot.last_input_us > 20000000;
        if (should_dim != dimmed) {
            bsp_display_backlight(should_dim ? 15 : 70);
            dimmed = should_dim;
        }
        if (changed && bsp_lvgl_lock(100)) {
            bgm_ui_update(&snapshot.model, battery);
            bsp_lvgl_unlock();
        }
    }
    xSemaphoreGive(s_ui_stopped);
    vTaskSuspend(NULL);
}

static void release_workers(void) {
    if (s_audio_task) vTaskDelete(s_audio_task);
    if (s_ui_task) vTaskDelete(s_ui_task);
    s_audio_task = s_ui_task = NULL;
    if (s_commands) vQueueDelete(s_commands);
    if (s_snapshots) vQueueDelete(s_snapshots);
    s_commands = s_snapshots = NULL;
    if (s_audio_stopped) vSemaphoreDelete(s_audio_stopped);
    if (s_ui_stopped) vSemaphoreDelete(s_ui_stopped);
    s_audio_stopped = s_ui_stopped = NULL;
}

void queen_bgm_enter(void) {
    s_ui_ready = bgm_ui_create();
}

void queen_bgm_exit(void) {
    // Called under the LVGL lock, after stop has joined both workers.
    if (s_audio_task || s_ui_task) return;
    bgm_ui_destroy();
    s_ui_ready = false;
}

esp_err_t queen_bgm_start(void) {
    if (s_audio_task || s_ui_task) return ESP_ERR_INVALID_STATE;
    if (!s_ui_ready) return ESP_FAIL;
    s_stop_requested = false;
    s_audio_acked = s_ui_acked = false;
    s_stop_result = ESP_OK;
    s_audio_ready = bsp_audio_init() == ESP_OK;
    s_battery_ready = bsp_battery_init() == ESP_OK;
    s_commands = xQueueCreate(16, sizeof(bgm_action_t));
    s_snapshots = xQueueCreate(1, sizeof(snapshot_t));
    s_audio_stopped = xSemaphoreCreateBinary();
    s_ui_stopped = xSemaphoreCreateBinary();
    if (!s_commands || !s_snapshots || !s_audio_stopped || !s_ui_stopped ||
        xTaskCreate(audio_task, "bgm_audio", 8192, NULL, 7, &s_audio_task) != pdPASS ||
        xTaskCreate(ui_task, "bgm_ui", 4096, NULL, 3, &s_ui_task) != pdPASS) {
        // Tasks have not been activated and own no peripherals or UI yet.
        release_workers();
        if (bsp_lvgl_lock(500)) {
            bgm_ui_fault("Worker start failed; restart device");
            bsp_lvgl_unlock();
        }
        return ESP_ERR_NO_MEM;
    }
    bsp_display_backlight(70);
    xTaskNotifyGive(s_audio_task);
    xTaskNotifyGive(s_ui_task);
    ESP_LOGI(TAG, "Queen Reborn BGM entered; six offline tracks");
    return ESP_OK;
}

esp_err_t queen_bgm_stop(void) {
    if (!s_audio_task && !s_ui_task) return ESP_OK;
    s_stop_requested = true;
    // Remember each consumed acknowledgement across retries. A timeout keeps
    // every handle/queue/screen alive; no producer is force-deleted mid-write.
    if (!s_audio_acked) s_audio_acked = xSemaphoreTake(s_audio_stopped, pdMS_TO_TICKS(2000)) == pdTRUE;
    if (!s_ui_acked) s_ui_acked = xSemaphoreTake(s_ui_stopped, pdMS_TO_TICKS(2000)) == pdTRUE;
    if (!s_audio_acked || !s_ui_acked) return ESP_ERR_TIMEOUT;
    if (s_stop_result != ESP_OK) {
        s_stop_result = bsp_audio_sleep();
        if (s_stop_result != ESP_OK) return s_stop_result;
    }
    release_workers();
    bsp_display_backlight(100);
    ESP_LOGI(TAG, "Queen Reborn BGM stopped; workers released");
    return ESP_OK;
}
