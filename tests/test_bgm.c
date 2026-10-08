#include "bgm_model.h"
#include "bgm_adpcm.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    bool available[BGM_TRACK_COUNT] = {true,true,false,true,true,true};
    bgm_model_t model;
    bgm_model_init(&model);
    assert(model.track == 0 && model.volume == 45 && model.repeat && !model.playing);
    bgm_model_apply(&model, BGM_PREVIOUS, available);
    assert(model.track == 5);
    bgm_model_apply(&model, BGM_NEXT, available);
    assert(model.track == 0);
    bgm_model_apply(&model, BGM_TOGGLE, available);
    model.position = 16000;
    assert(model.playing);
    bgm_model_apply(&model, BGM_TOGGLE, available);
    assert(!model.playing && model.position == 16000);
    bgm_model_apply(&model, BGM_TOGGLE, available);
    assert(model.playing && model.position == 16000);
    bgm_model_apply(&model, BGM_NEXT, available);
    assert(model.track == 1 && model.playing && model.position == 0);
    bgm_model_apply(&model, BGM_NEXT, available);
    assert(model.track == 2 && !model.playing);
    bgm_model_apply(&model, BGM_TOGGLE, available);
    assert(!model.playing); /* An absent track must never start fake playback. */
    bgm_model_apply(&model, BGM_NEXT, available);
    bgm_model_apply(&model, BGM_TOGGLE, available);
    model.position = 12345;
    bgm_model_apply(&model, BGM_FINISHED, available);
    assert(model.playing && model.position == 0);
    bgm_model_apply(&model, BGM_REPEAT, available);
    bgm_model_apply(&model, BGM_FINISHED, available);
    assert(!model.playing && model.position == 0);
    bgm_model_apply(&model, BGM_FAILED, available);
    assert(model.error && !model.playing);
    bgm_model_apply(&model, BGM_TOGGLE, available);
    assert(!model.error && model.playing);
    for (int i = 0; i < 50; i++) bgm_model_apply(&model, BGM_VOLUME_UP, available);
    assert(model.volume == 100);
    for (int i = 0; i < 50; i++) bgm_model_apply(&model, BGM_VOLUME_DOWN, available);
    assert(model.volume == 0);

    const uint8_t codes[] = {0x10,0x32,0x54,0x76};
    const int16_t expected[] = {0,1,4,8,15,27,47,88};
    int16_t pcm[8] = {0}, chunked[8] = {0};
    bgm_adpcm_t decoder;
    bgm_adpcm_reset(&decoder);
    assert(bgm_adpcm_decode(&decoder, codes, sizeof(codes), 8, pcm, 8) == 8);
    assert(memcmp(pcm, expected, sizeof(expected)) == 0);
    assert(bgm_adpcm_decode(&decoder, codes, sizeof(codes), 8, pcm, 8) == 0);
    bgm_adpcm_reset(&decoder);
    assert(bgm_adpcm_decode(&decoder, codes, sizeof(codes), 7, chunked, 3) == 3);
    assert(bgm_adpcm_decode(&decoder, codes, sizeof(codes), 7, chunked + 3, 5) == 4);
    assert(memcmp(chunked, expected, 7 * sizeof(int16_t)) == 0);
    bgm_adpcm_reset(&decoder);
    assert(bgm_adpcm_decode(&decoder, codes, 1, 100, pcm, 8) == 2);
    assert(bgm_adpcm_decode(&decoder, codes, 1, 100, pcm, 8) == 0);
    bgm_adpcm_reset(&decoder);
    decoder.index = 99;
    assert(bgm_adpcm_decode(&decoder, codes, sizeof(codes), 8, pcm, 8) == 0);
    uint8_t loud[24];
    memset(loud, 0x77, sizeof(loud));
    bgm_adpcm_reset(&decoder);
    for (int i = 0; i < 6; i++) assert(bgm_adpcm_decode(&decoder, loud, sizeof(loud), 48, pcm, 8) == 8);
    assert(pcm[7] == 32767 && decoder.index == 88);
    puts("BGM model/decoder: PASS");
    return 0;
}
