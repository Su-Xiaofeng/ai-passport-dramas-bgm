#include "bgm_adpcm.h"

static const int s_steps[89] = {
    7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,
    66,73,80,88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,
    371,408,449,494,544,598,658,724,796,876,963,1060,1166,1282,1411,1552,
    1707,1878,2066,2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,
    5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,15289,16818,
    18500,20350,22385,24623,27086,29794,32767
};
static const int s_index[8] = {-1,-1,-1,-1,2,4,6,8};

void bgm_adpcm_reset(bgm_adpcm_t *state) {
    *state = (bgm_adpcm_t){0};
}

size_t bgm_adpcm_decode(bgm_adpcm_t *state, const uint8_t *data,
                        size_t bytes, uint32_t samples,
                        int16_t *pcm, size_t capacity) {
    if (!state || !data || !pcm || state->index < 0 || state->index > 88) return 0;
    size_t n = 0;
    while (n < capacity && state->sample < samples && state->sample / 2 < bytes) {
        int code = (data[state->sample / 2] >> ((state->sample & 1) * 4)) & 15;
        int step = s_steps[state->index];
        int diff = step >> 3;
        if (code & 1) diff += step >> 2;
        if (code & 2) diff += step >> 1;
        if (code & 4) diff += step;
        state->predictor += (code & 8) ? -diff : diff;
        if (state->predictor > 32767) state->predictor = 32767;
        if (state->predictor < -32768) state->predictor = -32768;
        state->index += s_index[code & 7];
        if (state->index < 0) state->index = 0;
        if (state->index > 88) state->index = 88;
        pcm[n++] = (int16_t)state->predictor;
        state->sample++;
    }
    return n;
}
