#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BGM_TRACK_COUNT 6
#define BGM_SAMPLE_RATE 16000u

typedef enum {
    BGM_PREVIOUS, BGM_NEXT, BGM_TOGGLE, BGM_VOLUME_UP,
    BGM_VOLUME_DOWN, BGM_REPEAT, BGM_FINISHED, BGM_FAILED,
} bgm_action_t;

typedef struct {
    uint8_t track;
    uint8_t volume;
    bool playing;
    bool repeat;
    bool error;
    uint32_t position;
    uint32_t generation;
} bgm_model_t;

void bgm_model_init(bgm_model_t *model);
void bgm_model_apply(bgm_model_t *model, bgm_action_t action,
                     const bool available[BGM_TRACK_COUNT]);
