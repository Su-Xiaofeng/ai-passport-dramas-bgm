// Execute the real player lifecycle with deterministic RTOS scheduling.
#define bsp_audio_sleep test_base_sleep
#define bsp_audio_write test_base_write
#include "demo_stubs/demo_runtime.c"
#undef bsp_audio_sleep
#undef bsp_audio_write
#include "../main/bgm_ui.h"
#include "freertos/queue.h"

static unsigned sleep_calls, destroy_calls, queue_deletes, create_calls, fail_create_at;
static int sleep_result;
static bool ui_live;
static void (*write_hook)(void);
struct test_queue { bool deleted, available; size_t item_size; unsigned char data[128]; };
static struct test_queue queues[16];
static unsigned queue_count;

QueueHandle_t xQueueCreate(unsigned depth, size_t item_size) {
    (void)depth;
    assert(queue_count < 16 && item_size <= sizeof(queues[0].data));
    QueueHandle_t q = &queues[queue_count++];
    *q = (struct test_queue){ .item_size = item_size };
    return q;
}
BaseType_t xQueueSend(QueueHandle_t q, const void *data, TickType_t ticks) {
    (void)ticks;
    assert(q && !q->deleted);
    memcpy(q->data, data, q->item_size);
    q->available = true;
    return pdTRUE;
}
BaseType_t xQueueOverwrite(QueueHandle_t q, const void *data) { return xQueueSend(q, data, 0); }
BaseType_t xQueueReceive(QueueHandle_t q, void *data, TickType_t ticks) {
    (void)ticks;
    assert(q && !q->deleted);
    if (!q->available) return pdFALSE;
    memcpy(data, q->data, q->item_size);
    q->available = false;
    return pdTRUE;
}
void vQueueDelete(QueueHandle_t q) { assert(q && !q->deleted); q->deleted = true; queue_deletes++; }
void xTaskNotifyGive(TaskHandle_t task) { xTaskNotify(task, 1, eSetValueWithOverwrite); }
unsigned ulTaskNotifyTake(BaseType_t clear, TickType_t ticks) {
    (void)clear; (void)ticks;
    assert(test_current_task && test_current_task->notification);
    test_current_task->notification = 0;
    return 1;
}
unsigned uxTaskGetStackHighWaterMark(TaskHandle_t task) { (void)task; return 5000; }
esp_err_t bsp_audio_init(void) { return ESP_OK; }
esp_err_t bsp_battery_init(void) { return ESP_OK; }
int bsp_battery_soc(void) { return 80; }
esp_err_t bsp_audio_sleep(void) { sleep_calls++; return sleep_result; }
esp_err_t bsp_audio_write(const void *pcm, size_t bytes) {
    if (write_hook) write_hook();
    return test_base_write(pcm, bytes);
}
bool bgm_ui_create(void) { assert(!ui_live); ui_live = true; return true; }
void bgm_ui_destroy(void) { assert(ui_live); ui_live = false; destroy_calls++; }
void bgm_ui_update(const bgm_model_t *model, int battery) { (void)model; (void)battery; assert(ui_live); }
void bgm_ui_fault(const char *message) { (void)message; assert(ui_live); }
static BaseType_t create_worker(TaskFunction_t entry, const char *name, unsigned stack,
                               void *arg, unsigned priority, TaskHandle_t *task) {
    if (++create_calls == fail_create_at) return pdFALSE;
    return xTaskCreate(entry, name, stack, arg, priority, task);
}
#define xTaskCreate create_worker
#include "../main/queen_bgm.c"
#undef xTaskCreate

static const uint8_t silence[] = { 0, 0 };
const bgm_track_t bgm_tracks[BGM_TRACK_COUNT] = {
    { .title = "test", .data = silence, .bytes = 2, .samples = 4 }
};
static void stop_during_write(void) { s_stop_requested = true; }

int main(void) {
    queen_bgm_enter();
    fail_create_at = 2; // The first worker exists but has not been activated.
    assert(queen_bgm_start() == ESP_ERR_NO_MEM);
    assert(!s_audio_task && !s_ui_task && !s_commands && !s_snapshots);
    assert(test_task_deletes == 1 && queue_deletes == 2);
    assert(queen_bgm_stop() == ESP_OK);
    queen_bgm_exit();
    assert(!ui_live);

    fail_create_at = 0;
    queen_bgm_enter();
    assert(queen_bgm_start() == ESP_OK);
    TaskHandle_t audio = s_audio_task, ui = s_ui_task;
    queen_bgm_key(BSP_BTN_OK, BSP_BTN_DOUBLE);
    assert(!s_commands->available); // Double OK has no action.
    queen_bgm_key(BSP_BTN_OK, BSP_BTN_LONG);
    bgm_action_t action;
    assert(xQueueReceive(s_commands, &action, 0) && action == BGM_REPEAT);
    queen_bgm_key(BSP_BTN_OK, BSP_BTN_CLICK);
    write_hook = stop_during_write;
    test_run_worker(audio); // Stop request arrives while the worker owns PCM.
    assert(test_write_calls > 0 && s_audio_stopped->available);
    assert(queen_bgm_stop() == ESP_ERR_TIMEOUT); // UI has not acknowledged.
    assert(s_audio_acked && !s_ui_acked && !audio->deleted && !ui->deleted);
    queen_bgm_exit(); // Premature exit cannot delete objects used by a worker.
    assert(ui_live);
    test_run_worker(ui);
    assert(queen_bgm_stop() == ESP_OK); // Consumed audio ack survives retry.
    assert(audio->deleted && ui->deleted && !s_commands && !s_snapshots);
    queen_bgm_exit();
    assert(!ui_live);

    queen_bgm_enter();
    assert(queen_bgm_start() == ESP_OK);
    assert(s_audio_task != audio && s_ui_task != ui);
    assert(queen_bgm_stop() == ESP_ERR_TIMEOUT);
    sleep_result = ESP_FAIL;
    test_run_worker(s_audio_task);
    test_run_worker(s_ui_task);
    assert(queen_bgm_stop() == ESP_FAIL); // Codec failure also retains ownership.
    assert(ui_live && s_audio_task && s_ui_task);
    sleep_result = ESP_OK;
    assert(queen_bgm_stop() == ESP_OK);
    queen_bgm_exit();
    assert(queen_bgm_stop() == ESP_OK);
    assert(destroy_calls == 3 && queue_deletes == queue_count);
    assert(sleep_calls > 0);
    puts("BGM enter/play/stop/re-enter and failure-retry tests: PASS");
    return 0;
}
