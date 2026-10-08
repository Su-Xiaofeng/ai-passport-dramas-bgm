#pragma once
#include "bsp_button.h"

#define QUEEN_BGM_TITLE "重生之我是女王 BGM"
// enter/exit require the LVGL lock; start/stop run without it.
void queen_bgm_enter(void);
void queen_bgm_exit(void);
void queen_bgm_key(bsp_btn_t btn, bsp_btn_ev_t event);
esp_err_t queen_bgm_start(void);
esp_err_t queen_bgm_stop(void);
