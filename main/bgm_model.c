#include "bgm_model.h"

void bgm_model_init(bgm_model_t *model) {
    *model = (bgm_model_t){ .volume = 45, .repeat = true };
}

void bgm_model_apply(bgm_model_t *model, bgm_action_t action,
                     const bool available[BGM_TRACK_COUNT]) {
    switch (action) {
    case BGM_PREVIOUS:
    case BGM_NEXT:
        model->track = (uint8_t)((model->track +
            (action == BGM_NEXT ? 1 : BGM_TRACK_COUNT - 1)) % BGM_TRACK_COUNT);
        model->position = 0;
        model->generation++;
        model->error = false;
        if (!available[model->track]) model->playing = false;
        break;
    case BGM_TOGGLE:
        model->error = false;
        if (!available[model->track]) {
            model->playing = false;
        } else {
            model->playing = !model->playing;
        }
        break;
    case BGM_VOLUME_UP:
        model->volume = model->volume > 90 ? 100 : model->volume + 10;
        break;
    case BGM_VOLUME_DOWN:
        model->volume = model->volume < 10 ? 0 : model->volume - 10;
        break;
    case BGM_REPEAT:
        model->repeat = !model->repeat;
        break;
    case BGM_FINISHED:
        model->position = 0;
        model->generation++;
        if (!model->repeat) model->playing = false;
        break;
    case BGM_FAILED:
        model->playing = false;
        model->error = true;
        break;
    }
}
