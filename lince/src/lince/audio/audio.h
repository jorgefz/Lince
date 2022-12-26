
/*

    LINCE AUDIO API


Example Program:

    // On Init

    LinceAudioEngine* audio_engine;
    LinceSoundManager* sound_manager;
    LinceSound *music;

    LinceSoundConfig sound_config = LinceGetDefaultSoundConfig();
    sound_config.volume = 0.5f;

    audio_engine = LinceCreateAudioEngine();
    sound_manager = LinceCreateSoundManager(audio_engine, "meow.wav");
    music = LinceLoadStream(audio_engine, "music.wav");

    // -- play background music
    
    music->config.volume = 0.3f;
    LinceUpdateSound(music);
    LincePlaySound(music);

    // On Update

    if( nk_button(...) ){
        // Play a sound
        LinceSpawnSound(audio_engine, sound_manager, &sound_config);
    }

    // On Terminate

    LinceDeleteSoundManager(sound_manager);
    LinceDeleteSound(music);
    LinceTerminateAudioEngine(audio_engine);

*/



#ifndef LINCE_AUDIO_H
#define LINCE_AUDIO_H

#include <lince/core/core.h>
#include <lince/containers/array.h>

/*
Buffered sounds are entirely loaded onto memory - best for sounds < 5 sec
Streamed sounds are simultaneously read and played- best for sounds > 5 sec
*/
typedef enum LinceSoundType {
    LinceSound_Buffer,
    LinceSound_Stream
} LinceSoundType;

typedef struct LinceSoundConfig {
    float volume;    // Sound volume in dB
    float pitch;     // Frequency offset. A higher value makes the sound higher frequency.
    float pan;       // Which speaker dominates, from left (-1.0) to both (0.0) to right (1.0).
    LinceBool loop;  // Switch whether sound should loop over and play continuously
} LinceSoundConfig;

/*
Sound instance that plays an audio file.
To play the same sound simultaneously, create several LinceSound objects,
or use the LinceSoundManager.
*/
typedef struct LinceSound{
    void* handle;    // ma_sound
    char* filename;
    enum LinceSoundType type;
    LinceSoundConfig config;
} LinceSound;

/*
Stores sound objects from the same sound file for simultaneous playing
*/
typedef struct LinceSoundManager{
    array_t sound_cache; // array<LinceSound>
    char* filename;
    enum LinceSoundType type;
} LinceSoundManager;

typedef struct LinceAudioEngine {
    void* handle; // ma_engine
} LinceAudioEngine;

/*
Default initialisation settings for sounds
*/
LinceSoundConfig LinceGetDefaultSoundConfig(void);

/*
Initialises a given LinceSound object using the given handle.
The filename and config must be provided through that object.
*/
void LinceInitSound(LinceAudioEngine* audio, LinceSound* s);

/*
Creates a buffered sound from file and returns a heap-allocated pointer to it.
To play it, use 'LincePlaySound'.
Don't forget to delete it with 'LinceDeleteSound()'.
*/
LinceSound* LinceLoadSound(LinceAudioEngine* audio, const char* filename, LinceSoundConfig* config);

/*
Creates a streamed sound from file and returns a heap-allocated pointer to it,
similarly to LinceLoadSound.
*/
LinceSound* LinceLoadStream(LinceAudioEngine* audio, const char* filename, LinceSoundConfig* config);

/*
Uninitialises and deallocates a sound object.
*/
void LinceDeleteSound(LinceSound* s);

/*
Controls the sound cursor to start, stop, and rewind a sound.
*/
void LincePlaySound(LinceSound* s);
void LinceStopSound(LinceSound* s);
void LinceRewindSound(LinceSound* s);

/*
Returns the status of a sound.
Note: sound is 'stopped' when it is not playing but not finished either.
*/
LinceBool LinceIsSoundStopped(LinceSound* s);
LinceBool LinceIsSoundFinished(LinceSound* s);
LinceBool LinceIsSoundPlaying(LinceSound* s);

/*
The sound config can be updated on the fly via the sound object,
e.g. sound->config.volume = 1.0f.
This function must be called for these changes to have an effect.
*/
void LinceUpdateSound(LinceSound* s);

/*
The sound manager allows for the same sound file to be played simultaneously on the fly.
It dynamically creates and reuses sound objects each time a sound is requested.
The manager must be uninitialised as it is heap-allocated.
*/
LinceSoundManager* LinceCreateSoundManager(LinceAudioEngine* audio, LinceSoundType type, const char* filename);
void LinceDeleteSoundManager(LinceSoundManager* manager);

/*
Used to play a sound managed by the sound manager.
The starting configuration for the sound (e.g. volume) can be specified as well.
*/
void LinceSpawnSound(LinceAudioEngine* audio, LinceSoundManager* manager, LinceSoundConfig* config);

/*
Stops all the sounds owned by a sound manager,
which flags them for reuse.
*/
void LinceStopAllManagerSounds(LinceSoundManager* manager);

/*
The audio engine is used by the Miniaudio backend to open sound files,
communicate with the OS, and keep track of all loaded sounds.
It must be created before any sound is loaded, and destroyed when all sounds have been deleted.
*/
LinceAudioEngine* LinceCreateAudioEngine(void);
void LinceDeleteAudioEngine(LinceAudioEngine* audio);


#endif /* LINCE_AUDIO_H */
