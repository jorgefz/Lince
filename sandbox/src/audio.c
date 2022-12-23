

#include <miniaudio.h>

#include "audio.h"
#include <lince/core/core.h>
#include <lince/core/memory.h>

// Creates ma_sound handle from filename and config passed through provided object
int LinceCreateSoundInstance(LinceAudioManager* audio, LinceSound* s){
    if(!s || !s->filename) return -1;

    ma_result result;
    uint32_t flags = MA_SOUND_FLAG_DECODE;
    
    if(s->type == LinceSound_Stream){
        flags |= MA_SOUND_FLAG_STREAM;
    }

    s->handle = LinceMalloc(sizeof(ma_sound));
    LINCE_ASSERT_ALLOC(s->handle, sizeof(ma_sound));
    
    result = ma_sound_init_from_file(&audio->engine, s->filename, flags, NULL, NULL, s->handle);
    LINCE_ASSERT(result == MA_SUCCESS, "Failed to load sound '%s'", s->filename);

    LinceUpdateSoundInstance(s);

    return 0;
}

// Uninitialises provided sound object
void LinceDeleteSoundInstance(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
}

// Applies settings
void LinceUpdateSoundInstance(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
    ma_sound_set_volume(s->handle, s->volume);
    ma_sound_set_looping(s->handle, s->loop);
}

void LincePlaySoundInstance(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
    ma_sound_start(s->handle);
}

void LinceStopSoundInstance(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
    ma_sound_stop(s->handle);
}

void LinceRewindSoundInstance(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
    ma_sound_seek_to_pcm_frame(s->handle, 0);
}

LinceBool LinceIsSoundFinished(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
    return (LinceBool)ma_sound_at_end(s->handle);
}

LinceBool LinceIsSoundPlaying(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
    return (LinceBool)ma_sound_is_playing(s->handle);
}

LinceBool LinceIsSoundStopped(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
    return (LinceBool)( !LinceIsSoundPlaying(s) && !LinceIsSoundFinished(s) );
}


void LinceInitAudioManager(LinceAudioManager* audio){
    ma_result result;
    result = ma_engine_init(NULL, &audio->engine);
    LINCE_ASSERT(result == MA_SUCCESS, "Failed to initialise audio engine");
    audio->sounds = array_create(sizeof(LinceSound));
}

void LinceTerminateAudioManager(LinceAudioManager* audio){
    ma_engine_uninit(&audio->engine);
    // ma_sound instances get automatically destroyed when the engine is deleted
    for(uint32_t i = 0; i != audio->sounds.size; ++i){
        LinceSound* s = array_get(&audio->sounds, i);
        LinceFree(s->filename);
        //ma_sound_stop(&s->handle);
        //ma_sound_uninit(&s->handle);
    }
    array_destroy(&audio->sounds);
}

LinceSoundCollection* LinceInitSoundCollection(LinceSoundType type, const char* filename){
    LinceSoundCollection collection = {
        .type = type,
        .filename = LinceNewCopy(filename, strlen(filename) + 1),
        .sounds = array_create(sizeof(LinceSound))
    };
    return LinceNewCopy(&collection, sizeof(LinceSoundCollection));
}

void LinceDeleteSoundCollection(LinceSoundCollection* sc){
    LINCE_ASSERT(sc, "NULL pointer");
    if(sc->filename) LinceFree(sc->filename);
    array_destroy(&sc->sounds);
    LinceFree(sc); 
}

void LinceSpawnSound(LinceAudioManager* audio, LinceSoundCollection* sc, LinceSound* config){
    LINCE_ASSERT(sc && sc->filename, "NULL pointer");

    // Reuse stored sound that has finished
    for(uint32_t i = 0; i != sc->sounds.size; ++i){
        LinceSound* s = array_get(&sc->sounds, i);
        if(LinceIsSoundFinished(s)){
            LinceRewindSoundInstance(s);
            LincePlaySoundInstance(s);
            printf("Sound reused (%d sounds)\n",  sc->sounds.size);
            return;
        }
    }

    // Load new sound if none are available
    LinceSound new_sound = {
        .filename = sc->filename,
        .type = sc->type,
        .volume = config ? config->volume : 0.5f
    };

    if(sc->sounds.size == 0){
        LinceCreateSoundInstance(audio, &new_sound);
    } else {
        // Avoid loading from file to reduce overhead
        LinceSound *first = array_get(&sc->sounds, 0);
        int flags = MA_SOUND_FLAG_DECODE;
        if(sc->type == LinceSound_Stream){
            flags |= MA_SOUND_FLAG_STREAM;
        }
        new_sound.handle = LinceMalloc(sizeof(ma_sound));
        ma_sound_init_copy(&audio->engine, first->handle, flags, NULL, new_sound.handle);
    }

    array_push_back(&sc->sounds, &new_sound);
    LinceRewindSoundInstance(&new_sound);
    LincePlaySoundInstance(&new_sound);
    printf("Sound created (%u sounds)\n", sc->sounds.size);
}

/*
int32_t LinceLoadSound(LinceAudioManager* audio, const char* filename){
    LinceSound sound = {
        .filename = LinceNewCopy(filename, (strlen(filename)+1)*sizeof(char)),
        .type = LinceSound_Buffer,
        .volume = 0.5f,
        .handle = LinceMalloc(sizeof(ma_sound))
    };
    array_push_back(&audio->sounds, &sound);
    // This preloads buffered data source into memory before the engine
    LinceSound *s = array_back(&audio->sounds);
    ma_result result = ma_sound_init_from_file(&audio->engine, filename, MA_SOUND_FLAG_DECODE, NULL, NULL, s->handle);
    return (int32_t)audio->sounds.size - 1;
}

int32_t LinceLoadStream(LinceAudioManager* audio, const char* filename){
    LinceSound sound = {
        .filename = LinceNewCopy(filename,  (strlen(filename)+1)*sizeof(char)),
        .type = LinceSound_Stream,
        .volume = 0.5f,
        .handle = LinceMalloc(sizeof(ma_sound))
    };
    array_push_back(&audio->sounds, &sound);
    LinceSound *s = array_back(&audio->sounds);
    ma_sound_init_from_file(&audio->engine, filename, MA_SOUND_FLAG_STREAM, NULL, NULL, s->handle);
    return (int32_t)audio->sounds.size - 1;
}
*/

