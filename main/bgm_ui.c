#include "bgm_ui.h"
#include "queen_bgm.h"
#include "bgm_tracks.h"
#include "lvgl.h"
#include "esp_log.h"
#include "../assets/fonts/bgm_glyphs.h"

LV_FONT_DECLARE(bgm_font_12);
LV_FONT_DECLARE(bgm_font_16);
LV_FONT_DECLARE(bgm_font_20);
#define GOLD 0xE0BE75
#define INK 0x0D1017
#define MUTED 0x959389
#define WHITE 0xF5F0E7

static lv_obj_t *s_fault_screen;
static lv_obj_t *s_screen, *s_title, *s_status, *s_time;
static lv_obj_t *s_number, *s_battery, *s_mode, *s_progress, *s_volume;
static lv_obj_t *s_bars[7];

static lv_obj_t *box(lv_obj_t *parent, int x, int y, int w, int h, uint32_t color) {
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    return obj;
}

static lv_obj_t *label(int x, int y, int w, const char *text, const lv_font_t *font, uint32_t color) {
    lv_obj_t *obj = lv_label_create(s_screen);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_width(obj, w);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(color), 0);
    lv_label_set_text(obj, text);
    return obj;
}

void bgm_ui_fault(const char *message) {
    if (s_fault_screen) lv_obj_delete(s_fault_screen);
    lv_obj_t *screen = s_fault_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(INK), 0);
    lv_obj_t *obj = lv_label_create(screen);
    lv_obj_set_width(obj, 200);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(WHITE), 0);
    lv_label_set_text(obj, message);
    lv_obj_center(obj);
    lv_screen_load(screen);
}

static bool fonts_cover_ui(void) {
    const lv_font_t *fonts[] = {&bgm_font_12, &bgm_font_16, &bgm_font_20};
    for (size_t f = 0; f < 3; f++) {
        for (size_t i = 0; i < sizeof(bgm_glyphs) / sizeof(bgm_glyphs[0]); i++) {
            lv_font_glyph_dsc_t dsc = {0};
            if (!lv_font_get_glyph_dsc(fonts[f], &dsc, bgm_glyphs[i], 0) || dsc.is_placeholder) {
                ESP_LOGE("bgm_font", "Missing U+%04lx at font %u", (unsigned long)bgm_glyphs[i], (unsigned)f);
                return false;
            }
        }
        lv_font_glyph_dsc_t missing = {0};
        if (lv_font_get_glyph_dsc(fonts[f], &missing, 0x9F98, 0) && !missing.is_placeholder) return false;
    }
    return true;
}

bool bgm_ui_create(void) {
    if (!fonts_cover_ui()) { bgm_ui_fault("Font coverage failed"); return false; }
    s_screen = lv_obj_create(NULL);
    lv_obj_remove_style_all(s_screen);
    lv_obj_set_style_bg_color(s_screen, lv_color_hex(INK), 0);
    lv_obj_set_style_bg_opa(s_screen, LV_OPA_COVER, 0);
    lv_obj_remove_flag(s_screen, LV_OBJ_FLAG_SCROLLABLE);
    label(20, 15, 158, QUEEN_BGM_TITLE, &bgm_font_16, GOLD);
    s_battery = label(181, 19, 40, "--%", &bgm_font_12, MUTED);
    lv_obj_set_style_text_align(s_battery, LV_TEXT_ALIGN_RIGHT, 0);
    box(s_screen, 20, 41, 200, 1, 0x3B352A);
    label(20, 51, 140, "气场全开", &bgm_font_20, WHITE);
    s_number = label(174, 57, 47, "01/06", &bgm_font_12, GOLD);
    lv_obj_t *disc = box(s_screen, 81, 85, 78, 78, 0x191C23);
    lv_obj_set_style_radius(disc, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(disc, 2, 0);
    lv_obj_set_style_border_color(disc, lv_color_hex(GOLD), 0);
    lv_obj_t *inner = box(disc, 27, 27, 20, 20, GOLD);
    lv_obj_set_style_radius(inner, LV_RADIUS_CIRCLE, 0);
    for (int i = 0; i < 7; i++) {
        s_bars[i] = box(s_screen, 28 + i * 6, 135, 3, 8, GOLD);
        lv_obj_set_style_radius(s_bars[i], 1, 0);
    }
    label(169, 100, 48, "BGM", &bgm_font_12, GOLD);
    s_mode = label(165, 122, 62, "单曲循环", &bgm_font_12, MUTED);
    s_title = label(20, 176, 200, "", &bgm_font_16, WHITE);
    lv_obj_set_style_text_align(s_title, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(s_title, LV_LABEL_LONG_WRAP);
    lv_obj_set_height(s_title, 43);
    s_status = label(20, 221, 200, "待导入音频", &bgm_font_12, GOLD);
    lv_obj_set_style_text_align(s_status, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_t *track = box(s_screen, 20, 243, 200, 3, 0x34332D);
    s_progress = box(track, 0, 0, 1, 3, GOLD);
    s_time = label(20, 252, 116, "00:00 / --:--", &bgm_font_12, MUTED);
    s_volume = label(153, 252, 68, "音量 45", &bgm_font_12, MUTED);
    lv_obj_set_style_text_align(s_volume, LV_TEXT_ALIGN_RIGHT, 0);
    label(20, 271, 206, "上/下 切歌  OK 播放/暂停", &bgm_font_12, WHITE);
    label(20, 291, 206, "长按 上/下 音量  OK 循环", &bgm_font_12, MUTED);
    bgm_model_t initial;
    bgm_model_init(&initial);
    bgm_ui_update(&initial, -1);
    lv_screen_load(s_screen);
    return true;
}

void bgm_ui_update(const bgm_model_t *model, int battery) {
    if (!s_screen || s_fault_screen) return;
    const bgm_track_t *track = &bgm_tracks[model->track];
    lv_label_set_text(s_title, track->title);
    lv_label_set_text_fmt(s_number, "%02u/06", model->track + 1);
    lv_label_set_text_fmt(s_volume, "音量 %u", model->volume);
    if (battery >= 0) lv_label_set_text_fmt(s_battery, "%d%%", battery);
    else lv_label_set_text(s_battery, "--%");
    lv_label_set_text(s_mode, model->repeat ? "单曲循环" : "播放一次");
    lv_label_set_text(s_status, model->error ? "播放失败，请重试" : !track->samples ? "待导入音频" :
        model->playing ? "正在播放" : model->position ? "已暂停" : "按 OK 开启气场");
    uint32_t elapsed = model->position / BGM_SAMPLE_RATE, duration = track->samples / BGM_SAMPLE_RATE;
    if (track->samples) {
        lv_label_set_text_fmt(s_time, "%02lu:%02lu / %02lu:%02lu", (unsigned long)(elapsed / 60),
            (unsigned long)(elapsed % 60), (unsigned long)(duration / 60), (unsigned long)(duration % 60));
    } else lv_label_set_text(s_time, "00:00 / --:--");
    uint32_t width = track->samples ? (uint32_t)((uint64_t)model->position * 200 / track->samples) : 0;
    lv_obj_set_width(s_progress, width ? (int)width : 1);
    lv_obj_set_style_bg_opa(s_progress, width ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
    for (int i = 0; i < 7; i++) {
        int height = model->playing ? 8 + (int)((model->position / 1600 + i * 3) % 6) * 4 : 5;
        lv_obj_set_height(s_bars[i], height);
        lv_obj_set_y(s_bars[i], 145 - height);
    }
}

void bgm_ui_destroy(void) {
    if (s_fault_screen) lv_obj_delete(s_fault_screen);
    if (s_screen) lv_obj_delete(s_screen);
    s_fault_screen = s_screen = NULL;
}
