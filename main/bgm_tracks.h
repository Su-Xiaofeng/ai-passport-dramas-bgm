#pragma once
#include "bgm_model.h"

typedef struct {
    const char *title;
    const uint8_t *data;
    size_t bytes;
    uint32_t samples;
} bgm_track_t;

extern const bgm_track_t bgm_tracks[BGM_TRACK_COUNT];
