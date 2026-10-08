#pragma once
#include "bgm_model.h"
/* These APIs run only in LVGL context or while holding bsp_lvgl_lock(). */
bool bgm_ui_create(void);
void bgm_ui_update(const bgm_model_t *model, int battery);
void bgm_ui_fault(const char *message);
void bgm_ui_destroy(void);
