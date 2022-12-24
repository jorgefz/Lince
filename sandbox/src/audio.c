

#include <miniaudio.h>

#include "audio.h"
#include <lince/core/core.h>
#include <lince/core/memory.h>


static const LinceSoundConfig default_sound_config = {
    .volume = 1.0f, .pitch = 1.0f, .pan = 0.0f, .loop = LinceFalse
};


LinceSoundConfig LinceGetDefaultSoundConfig(void){
    return default_sound_config;
}

// Creates ma_sound handle from filename and config passed through provided object
void LinceInitSound(LinceAudioEngine* audio, LinceSound* s){
    LINCE_ASSERT(s && s->filename, "NULL pointer");

    ma_result result;
    uint32_t flags = MA_SOUND_FLAG_DECODE;
    
    if(s->type == LinceSound_Stream){
        flags |= MA_SOUND_FLAG_STREAM;
    }

    s->handle = LinceMalloc(sizeof(ma_sound));
    LINCE_ASSERT_ALLOC(s->handle, sizeof(ma_sound));
    
    result = ma_sound_init_from_file(audio->handle, s->filename, flags, NULL, NULL, s->handle);
    LINCE_ASSERT(result == MA_SUCCESS, "Failed to load sound '%s'", s->filename);

    LinceUpdateSound(s);
}


LinceSound* LinceLoadSound(LinceAudioEngine* audio, const char* filename, LinceSoundConfig* config){
    LinceSound sound = {
        .filename = LinceNewCopy(filename, (strlen(filename)+1)*sizeof(char)),
        .type = LinceSound_Buffer,
        .config = config ? (*config) : default_sound_config,
    };
    LinceInitSound(audio, &sound);
    return LinceNewCopy(&sound, sizeof(LinceSound));
}

LinceSound* LinceLoadStream(LinceAudioEngine* audio, const char* filename, LinceSoundConfig* config){
    LinceSound sound = {
        .filename = LinceNewCopy(filename, (strlen(filename)+1)*sizeof(char)),
        .type = LinceSound_Stream,
        .config = config ? (*config) : default_sound_config,
    };
    LinceInitSound(audio, &sound);
    return LinceNewCopy(&sound, sizeof(LinceSound));
}

// Uninitialises provided sound object
void LinceDeleteSound(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
    if(s->filename) LinceFree(s->filename);
    if(s->handle) LinceFree(s->handle);
}

// Applies settings
void LinceUpdateSound(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
    ma_sound_set_volume (s->handle, s->config.volume);
    ma_sound_set_looping(s->handle, s->config.loop);
    ma_sound_set_pitch  (s->handle, s->config.pitch);
    ma_sound_set_pan    (s->handle, s->config.pan);
}

void LincePlaySound(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
    ma_sound_start(s->handle);
}

void LinceStopSound(LinceSound* s){
    LINCE_ASSERT(s, "NULL pointer");
    ma_sound_stop(s->handle);
}

void LinceRewindSound(LinceSound* s){
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

LinceAudioEngine* LinceCreateAudioEngine(void){
    LinceAudioEngine engine = {
        .handle = LinceMalloc(sizeof(ma_engine))
    };
    ma_result result;
    result = ma_engine_init(NULL, engine.handle);
    LINCE_ASSERT(result == MA_SUCCESS, "Failed to initialise audio engine");
    return LinceNewCopy(&engine, sizeof(LinceAudioEngine));
}

void LinceDeleteAudioEngine(LinceAudioEngine* engine){
    LINCE_ASSERT(engine, "NULL pointer");
    ma_engine_uninit(engine->handle);
    LinceFree(engine->handle);
    LinceFree(engine);
}

LinceSoundManager* LinceCreateSoundManager(LinceAudioEngine* audio, LinceSoundType type, const char* filename){
    
    LinceSoundManager manager = {
        .type = type,
        .filename = LinceNewCopy(filename, strlen(filename) + 1),
        .sound_cache = array_create(sizeof(LinceSound))
    };
    
    // Pre-load sound file
    LinceSound first = {
        .handle = LinceMalloc(sizeof(ma_sound)),
        .filename = manager.filename,
        .type = type,
        .config = default_sound_config,
    };
    LinceInitSound(audio, &first);
    LinceStopSound(&first);
    array_push_back(&manager.sound_cache, &first);

    return LinceNewCopy(&manager, sizeof(LinceSoundManager));
}

void LinceDeleteSoundManager(LinceSoundManager* manager){
    LINCE_ASSERT(manager, "NULL pointer");
    if(manager->filename){
        LinceFree(manager->filename);
    }
    for(uint32_t i = 0; i != manager->sound_cache.size; ++i){
        LinceSound* s = array_get(&manager->sound_cache, i);
        if(s->handle){
            ma_sound_stop(s->handle);
            ma_sound_uninit(s->handle);
            LinceFree(s->handle);
        }
    }
    array_destroy(&manager->sound_cache);
    LinceFree(manager); 
}

void LinceSpawnSound(LinceAudioEngine* audio, LinceSoundManager* manager, LinceSoundConfig* config){
    LINCE_ASSERT(manager && manager->filename, "NULL pointer");

    // Reuse cached sound that is stopped or finished
    for(uint32_t i = 0; i != manager->sound_cache.size; ++i){
        LinceSound* s = array_get(&manager->sound_cache, i);
        if(LinceIsSoundStopped(s) || LinceIsSoundFinished(s)){
            if(config){
                memmove(&s->config, config, sizeof(LinceSoundConfig));
            } else {
                memmove(&s->config, &default_sound_config, sizeof(LinceSoundConfig));
            }
            LinceUpdateSound(s);
            LinceRewindSound(s);
            LincePlaySound(s);
            return;
        }
    }

    // Load new sound if none are available
    LinceSound new_sound = {
        .filename = manager->filename,
        .type = manager->type,
        .config = config ? (*config) : default_sound_config
    };

    if(manager->sound_cache.size == 0){
        LinceInitSound(audio, &new_sound);
    } else {
        // Avoid loading from file to reduce overhead
        // Note - this might not matter since it could already be cached on the engine
        LinceSound *first = array_get(&manager->sound_cache, 0);
        int flags = MA_SOUND_FLAG_DECODE;
        if(manager->type == LinceSound_Stream){
            flags |= MA_SOUND_FLAG_STREAM;
        }
        new_sound.handle = LinceMalloc(sizeof(ma_sound));
        ma_sound_init_copy(audio->handle, first->handle, flags, NULL, new_sound.handle);
    }

    array_push_back(&manager->sound_cache, &new_sound);
    LinceUpdateSound(&new_sound);
    LinceRewindSound(&new_sound);
    LincePlaySound(&new_sound);
}

void LinceStopAllManagerSounds(LinceSoundManager* manager){
    LINCE_ASSERT(manager, "NULL pointer");
    for(uint32_t i = 0; i != manager->sound_cache.size; ++i){
        LinceSound* s = array_get(&manager->sound_cache, i);
        LinceStopSound(s);
    }
}



