# Audio

## Example code
```c

LinceAudioEngine* audio_engine;
LinceSoundManager* sound_manager;
LinceSound *music;

void OnInit(){
    LinceSoundConfig sound_config = LinceGetDefaultSoundConfig();
    sound_config.volume = 0.5f;

    audio_engine = LinceCreateAudioEngine();
    sound_manager = LinceCreateSoundManager(audio_engine, "meow.wav");
    music = LinceLoadStream(audio_engine, "music.wav");

    // Play background music
    music->config.volume = 0.3f;
    LinceUpdateSound(music);
    LincePlaySound(music);
}

void OnUpdate(){
    if( nk_button(...) ){
        // Play a sound
        LinceSpawnSound(audio_engine, sound_manager, &sound_config);
    }
}

void OnTerminate(){
    LinceDeleteSoundManager(sound_manager);
    LinceDeleteSound(music);
    LinceTerminateAudioEngine(audio_engine);
}

```


## LinceSoundType

Types of sound:

- Buffered sounds are entirely loaded onto memory - best for sounds shorter than 5 sec
- Streamed sounds are simultaneously read and played - best for music or sounds longer than 5 sec

| Enum              | Value |
| ----------------- | ----- |
| LinceSound_Buffer |   0   |
| LinceSound_Stream |   1   |


## LinceSoundConfig

Structure that holds configuration settings that modify how a sound is played.

- `float volume`
    - Sound volume, positive number where a value of zero is silence
- `float pitch`
    - Frequency offset. A positive value makes the sound higher frequency, and a negative value reduces its frequency
- `float pan`     
    - Which speaker dominates, from left (-1.0) to both (0.0) to right (1.0)
- `LinceBool loop`
    - Switch whether sound should loop over and play continuously


## LinceSound

Main data structure for a single sound object that can be played. A single LinceSound can play a single sound only. To play the same sound simultaneously, load several LinceSound objects or use a sound manager (see below). 

- `void* handle`
    - Opaque pointer to an `ma_sound` object
- `char* filename`
    - Filename of the loaded audio file
- `enum LinceSoundType type`
    - Whether this instance is buffered or streamed
- `LinceSoundConfig config`
    - Settings that affect how the sound is played


## LinceSoundManager

Object that can spawn and play sound instances at will, allowing to play the same sound simultaneously without having to manually create and manage separate LinceSound instances. When spawning a sound, it first checks whether another cached sound instance is stopped or has finished playing and recycles it. Otherwise, it creates a new instance.

- `array_t sound_cache`
    - Array of cached LinceSounds
- `char* filename`
    - Filename of the audio file
- `enum LinceSoundType type`
    - Whether it manages buffered or streamed sounds


## LinceAudioEngine

The audio engine is used by the Miniaudio backend to open sound files, communicate with the OS, and keep track of all loaded sounds. It must be created before any sound is loaded, and destroyed when all sounds have been deleted.

- `void* handle`
    - Opaque pointer to `ma_engine` object



## LinceGetDefaultSoundConfig
```c
LinceSoundConfig LinceGetDefaultSoundConfig(void)
```
Returns the default initialisation settings for sounds.

Parameters: none
Returns:
* `LinceSoundConfig`: default sound configuration


## LinceInitSound
```c
void LinceInitSound(LinceAudioEngine* audio, LinceSound* s);
```
Initialises a given LinceSound object from a filename.

Parameters:
* `LinceAudioEngine* audio`: initialised audio engine
* `LinceSound* s`: sound object that must define a filename and a sound type. The configuration is optional. The `handle` parameter will be overwritten to point to an initialised sound object.

Returns: none


## LinceLoadSound
```c
LinceSound* LinceLoadSound(LinceAudioEngine* audio, const char* filename, LinceSoundConfig* config);
```
A helper function for `LinceInitSound` - creates a buffered sound from file and returns a heap-allocated pointer to it.
To play it, use `LincePlaySound`. Don't forget to delete it with `LinceDeleteSound()`.

Parameters:
* `LinceAudioEngine* audio`: initialised audio engine
* `char* filename`: String filename of audio file to load.
* `LinceSoundConfig* config`: Optional configuration. If `NULL`, the default configuration is used.

Returns:
* `LinceSound* sound`: loaded buffered sound instance.


## LinceLoadStream
```c
LinceSound* LinceLoadStream(LinceAudioEngine* audio, const char* filename, LinceSoundConfig* config);
```

A helper function for `LinceInitSound` - creates a streamed sound from file and returns a heap-allocated pointer to it.
To play it, use `LincePlaySound`. Don't forget to delete it with `LinceDeleteSound()`.

Parameters:
* `LinceAudioEngine* audio`: initialised audio engine
* `char* filename`: String filename of audio file to load.
* `LinceSoundConfig* config`: Optional configuration. If `NULL`, the default configuration is used.

Returns:
* `LinceSound* sound`: loaded streamed ssound instance.


## LinceDeleteSound
```c
void LinceDeleteSound(LinceSound* s);
```
Uninitialises and deallocates a sound object.

## LincePlaySound
```c
void LincePlaySound(LinceSound* s);
```
Starts or continues playing a sound instance.

## LinceStopSound
```c
void LinceStopSound(LinceSound* s);
```
Stops playing a sound instance.

## LinceRewindSound
```c
void LinceRewindSound(LinceSound* s);
```
Resets the cursos on a sound instance, so that it plays from the beginning the next time it's played.

## LinceIsSoundStopped
```c
LinceBool LinceIsSoundStopped(LinceSound* s);
```
Returns true if a sound has been stopped before finishing.

## LinceIsSoundFinished
```c
LinceBool LinceIsSoundFinished(LinceSound* s);
```
Returns true if a sound has finished playing. It will never return true if a sound is set to loop over.

## LinceIsSoundPlaying
```c
LinceBool LinceIsSoundPlaying(LinceSound* s);
```
Returns true if a sound is currently playing.

## LinceUpdateSound
```c
void LinceUpdateSound(LinceSound* s);
```
Applies the changes made through the sound config.
Since the sound config can be updated on the fly via the sound object (e.g. `sound->config.volume = 1.0f`), this function must be called for these changes to have an effect.


## LinceCreateSoundManager
```c
LinceSoundManager* LinceCreateSoundManager(LinceAudioEngine* audio, LinceSoundType type, const char* filename);
```
The sound manager allows for the same sound file to be played simultaneously on the fly. It dynamically creates and reuses sound objects each time a sound is requested. The manager must be uninitialised later as it is heap-allocated.

Parameters:
* `LinceAudioEngine* audio`: initialised audio engine
* `LinceSoundType type`: The type of sound (buffered or streamed) that it will manage.
* `char* filename`: String filename of audio file to load.

Returns:
* `LinceSoundManager* manager`: initialised sound manager object.

## LinceSpawnSound
```c
void LinceSpawnSound(LinceAudioEngine* audio, LinceSoundManager* manager, LinceSoundConfig* config);
```
Used to play a sound managed by the sound manager.
The starting configuration for the sound (e.g. volume) can be specified as well.

Parameters:
* `LinceAudioEngine* audio`: initialised audio engine
* `LinceSoundManager* manager`: initialised sound manager object.
* `LinceSoundConfig* config`: optional configuration for the newly spawned sound.

Returns: none

## LinceDeleteSoundManager
```c
void LinceDeleteSoundManager(LinceSoundManager* manager);
```
Frees an initialised sound manager.

## LinceStopAllManagerSounds
```c
void LinceStopAllManagerSounds(LinceSoundManager* manager);
```
Stops all the sounds owned by a sound manager, which flags them for reuse.

## LinceCreateAudioEngine
```c
LinceAudioEngine* LinceCreateAudioEngine(void);
```
Initialises an audio engine

## LinceDeleteAudioEngine
```c
void LinceDeleteAudioEngine(LinceAudioEngine* audio);
```
Deletes an audio engine
