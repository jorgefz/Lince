#ifndef LINCE_AUDIO_H
#define LINCE_AUDIO_H

#include <miniaudio.h>
#include <lince/core/core.h>
#include <lince/containers/array.h>

typedef enum LinceSoundType {
    LinceSound_Buffer, LinceSound_Stream
} LinceSoundType;

// Sound instance that plays an audio file
typedef struct LinceSound{
    ma_sound* handle;
    char* filename;
    enum LinceSoundType type;
    float volume;
    LinceBool loop;
} LinceSound;

// Stores sound objects from the same sound file for simultaneous playing
typedef struct LinceSoundCollection{
    array_t sounds; // array<LinceSound>
    char* filename;

    enum LinceSoundType type;
} LinceSoundCollection;

typedef struct LinceAudioManager {
    ma_engine engine;
    array_t sounds; // unused
} LinceAudioManager;

// Creates ma_sound handle from filename and config passed through provided object
int LinceCreateSoundInstance(LinceAudioManager* audio, LinceSound* s);

// Uninitialises provided sound object
void LinceDeleteSoundInstance(LinceSound* s);

void LinceUpdateSoundInstance(LinceSound* s);
void LincePlaySoundInstance(LinceSound* s);
void LinceStopSoundInstance(LinceSound* s);
void LinceRewindSoundInstance(LinceSound* s);

LinceBool LinceIsSoundStopped(LinceSound* s);
LinceBool LinceIsSoundFinished(LinceSound* s);
LinceBool LinceIsSoundPlaying(LinceSound* s);

LinceSoundCollection* LinceInitSoundCollection(LinceSoundType type, const char* filename);
void LinceDeleteSoundCollection(LinceSoundCollection* sc);

void LinceSpawnSound(LinceAudioManager* audio, LinceSoundCollection* sc, LinceSound* config);

void LinceInitAudioManager(LinceAudioManager* audio);
void LinceTerminateAudioManager(LinceAudioManager* audio);


#endif /* LINCE_AUDIO_H */
