/* Standalone Queen Reborn BGM. The playback page owns the screen until reboot. */
#include "queen_bgm.h"
#include "bgm_ui.h"
#include "bsp_button.h"
#include "bsp_display.h"
#include "esp_log.h"

static const char *TAG = "main";

// The shared timer callback enqueues only; codec and LVGL work stay in workers.
static void on_key(bsp_btn_t btn, bsp_btn_ev_t event, void *user) {
    (void)user;
    queen_bgm_key(btn, event);
}

void app_main(void) {
    ESP_LOGI(TAG, "重生之我是女王 BGM: direct boot, six offline tracks");
    if (bsp_display_init() != ESP_OK || !bsp_lvgl_init()) {
        ESP_LOGE(TAG, "Display initialization failed");
        return;
    }
    bsp_display_backlight(70);
    if (bsp_button_init(on_key, NULL) != ESP_OK) {
        if (bsp_lvgl_lock(500)) {
            bgm_ui_fault("Button initialization failed; restart device");
            bsp_lvgl_unlock();
        }
        return;
    }
    if (!bsp_lvgl_lock(1000)) return;
    queen_bgm_enter();
    bsp_lvgl_unlock();
    esp_err_t err = queen_bgm_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Player initialization failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "Standalone BGM ready; no demo navigation");
}
