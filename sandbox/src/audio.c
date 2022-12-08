

#include <miniaudio.h>

#include "audio.h"
#include <lince/core/core.h>
#include <lince/core/memory.h>


void LinceInitAudioManager(LinceAudioManager* audio){
    ma_result result;
    result = ma_engine_init(NULL, &audio->engine);
    LINCE_ASSERT(result == MA_SUCCESS, "Failed to initialise audio engine");

    array_t sounds = array_create(sizeof(ma_sound));
    audio->sounds = LinceNewCopy(&sounds, sizeof(array_t));
    LINCE_ASSERT(audio->sounds, "Failed to create sound array");
}

void LinceDeleteAudioManager(LinceAudioManager* audio){
    ma_engine_uninit(&audio->engine);

    for(uint32_t i = 0; i != audio->sounds->size; ++i){
        ma_sound* sound = array_get(audio->sounds, i);
        ma_sound_uninit(sound);
    }
    array_destroy(audio->sounds);
    LinceFree(audio->sounds);
    audio->sounds = NULL;
}

void LincePlaySound(LinceAudioManager* audio, const char* filename){
    ma_engine_play_sound(&audio->engine, filename, NULL);
}


/*
void LinceStreamSound(LinceAudioManager* audio, const char* filename){
    
    ma_sound *s;
    array_t* r = array_push_back(audio->sounds, NULL);
    s = array_back(audio->sounds);
    LINCE_ASSERT(r, "Failed to append value to array of size %u", audio->sounds->size);

    ma_sound_init_from_file(&audio->engine, filename, MA_SOUND_FLAG_STREAM, NULL, NULL, s);
    ma_sound_start(s);
}
*/
