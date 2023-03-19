
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

/** @enum LinceSoundType
* @brief Holds the type of a sound: buffered or streamed.
*/
typedef enum LinceSoundType {
    LinceSound_Buffer, //> Buffered sounds are entirely loaded onto memory - best for sounds < 5 sec
    LinceSound_Stream  //> Streamed sounds are simultaneously read and played- best for sounds > 5 sec
} LinceSoundType;

/** @struct LinceSoundConfig
* @brief Holds the settings on how a sound is played.
*/
typedef struct LinceSoundConfig {
    float volume;    //> Sound volume in dB
    float pitch;     //> Frequency offset. A higher value makes the sound higher frequency.
    float pan;       //> Which speaker dominates, from left (-1.0) to both (0.0) to right (1.0).
    LinceBool loop;  //> Switch whether sound should loop over and play continuously
} LinceSoundConfig;

/** @struct LinceSound
* @brief Sound instance that plays an audio file.
* To play the same sound simultaneously, create several LinceSound objects,
* or use the LinceSoundManager.
*/
typedef struct LinceSound{
    void* handle;             //> miniaudio handle for `ma_sound`
    char* filename;           //> File from which sound was loaded
    LinceSoundType type;      //> Sound type: streamed or buffered 
    LinceSoundConfig config;  //> Sound settings
} LinceSound;

/** @struct LinceSoundManager
* @brief Stores sound objects from the same sound file for simultaneous playing
*/
typedef struct LinceSoundManager{
    array_t sound_cache;      //> array_t<LinceSound>, sound instances
    char* filename;           //> Origin audio file
    enum LinceSoundType type; //> Sound type: streamed or buffered
} LinceSoundManager;

/** @struct LinceAudioEngine
* @brief Holds the underlying state for miniaudio sound management
*/
typedef struct LinceAudioEngine {
    void* handle; // miniaudio handle for `ma_engine`
} LinceAudioEngine;


/** @brief Initialises an audio engine.
* The audio engine is used by the Miniaudio backend to open sound files,
* communicate with the OS, and keep track of all loaded sounds.
* It must be created before any sound is loaded, and destroyed when all sounds have been deleted.
*/
LinceAudioEngine* LinceCreateAudioEngine(void);

/** @brief Terminates an audio engine and all of its sounds */
void LinceDeleteAudioEngine(LinceAudioEngine* audio);


/**
* Returns the default initialisation settings for sounds:
*   volume = 1.0f,
*   pitch  = 1.0f,
*   pan    = 0.0f,
*   loop   = false
*/
LinceSoundConfig LinceGetDefaultSoundConfig(void);


/**
* Initialises a given LinceSound object using the given handle.
* The filename (and the config, optionally) may be defined in the sound instance.
* @param audio Audio engine from `LinceCreateAudioEngine`.
* @param s Sound instance to initialise. Must have a filename defined.
*/
void LinceInitSound(LinceAudioEngine* audio, LinceSound* s);

/** @brief Allocates and returns a buffered sound from file.
* To play it, use `LincePlaySound`.
* Don't forget to delete it with `LinceDeleteSound`.
* @param audio Audio engine from `LinceCreateAudioEngine`.
* @param filename File from which to load audio
* @param config Settings for the sound
*/
LinceSound* LinceLoadSound(LinceAudioEngine* audio, const char* filename, LinceSoundConfig* config);

/** @brief Allocates and returns a streamed sound from file.
* Akin to `LinceLoadSound`.
* @param audio Audio engine from `LinceCreateAudioEngine`.
* @param filename File from which to stream audio
* @param config Settings for the sound
*/
LinceSound* LinceLoadStream(LinceAudioEngine* audio, const char* filename, LinceSoundConfig* config);

/** @brief Uninitialises and deallocates a sound instance, either buffered or streamed.
*/
void LinceDeleteSound(LinceSound* s);

/** @brief Starts or continues playing a sound */
void LincePlaySound(LinceSound* s);

/** @brief Pauses a sound */
void LinceStopSound(LinceSound* s);

/** @brief Rewinds sound to its beginning.
* If the sound was stopped, it does not autoplay it.
*/
void LinceRewindSound(LinceSound* s);

/** Returns the status of a sound.
* Note: sound is 'stopped' when it is not playing but not finished either.
*/

/** @brief Returns true if a sound has been manually stopped or hasn't started playing */
LinceBool LinceIsSoundStopped(LinceSound* s);

/** @brief Returns true if a sound has finished playing.
* Will never return true for sounds that loop forever.
*/
LinceBool LinceIsSoundFinished(LinceSound* s);

/** @brief Returns true if a sound is being played or streamed */
LinceBool LinceIsSoundPlaying(LinceSound* s);

/** @brief Applies changes made to a sound's settings via `sound.config`.
* The sound configuration can be updated on the fly via the sound object,
* e.g. sound->config.volume = 1.0f.
* This function must be called for these changes to have an effect.
*/
void LinceUpdateSound(LinceSound* s);

/** @brief Allocates and returns a sound manager.
* The sound manager allows for the same sound file to be played simultaneously on the fly.
* It dynamically creates and reuses sound objects each time a sound is requested.
* The manager must be deleted as it is heap-allocated.
* @param audio The audio engine
* @param type Streamed or buffered. It works best for buffered sounds.
* @param filename File from which to load audio.
*/
LinceSoundManager* LinceCreateSoundManager(LinceAudioEngine* audio, LinceSoundType type, const char* filename);

/** @brief Deallocates a sound manager */
void LinceDeleteSoundManager(LinceSoundManager* manager);

/** @brief Plays a sound using a sound manager.
* This allows the same audio to be played simultaneously.
* @param audio The audio engine.
* @param manager Audio manager initialised with the desired sound.
* @param config The starting configuration for the sound (e.g. volume). This cannot be changed later.
*/
void LinceSpawnSound(LinceAudioEngine* audio, LinceSoundManager* manager, LinceSoundConfig* config);

/** @brief Stops all the sounds owned by a sound manager, which flags them for reuse.
*/
void LinceStopAllManagerSounds(LinceSoundManager* manager);


#endif /* LINCE_AUDIO_H */
