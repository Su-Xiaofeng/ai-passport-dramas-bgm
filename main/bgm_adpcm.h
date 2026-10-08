#pragma once

#include <stddef.h>
#include <stdint.h>

/* Headerless IMA ADPCM, low nibble first, starting at predictor/index zero.
 * Total decoded sample count is carried in the track descriptor. */
typedef struct {
    int32_t predictor;
    int index;
    uint32_t sample;
} bgm_adpcm_t;

void bgm_adpcm_reset(bgm_adpcm_t *state);
size_t bgm_adpcm_decode(bgm_adpcm_t *state, const uint8_t *data,
                        size_t bytes, uint32_t samples,
                        int16_t *pcm, size_t capacity);
