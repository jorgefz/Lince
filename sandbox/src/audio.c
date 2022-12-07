

#include <miniaudio.h>

#include "audio.h"
#include <lince/core/core.h>
#include <lince/core/memory.h>


void LinceInitAudioManager(LinceAudioManager* audio){
    ma_result result;
    result = ma_engine_init(NULL, &audio->engine);
    LINCE_ASSERT(result == MA_SUCCESS, "Failed to initialise audio engine");
}

void LinceDeleteAudioManager(LinceAudioManager* audio){
    ma_engine_uninit(&audio->engine);
}

void LincePlaySound(LinceAudioManager* audio, const char* filename){
    ma_engine_play_sound(&audio->engine, filename, NULL);
}
