#ifndef LINCE_AUDIO_H
#define LINCE_AUDIO_H

#include <miniaudio.h>
#include <lince/containers/array.h>

typedef struct LinceAudioManager {
    ma_engine engine;
    array_t* sounds;
} LinceAudioManager;

void LinceInitAudioManager(LinceAudioManager* audio);

void LinceDeleteAudioManager(LinceAudioManager* audio);

void LincePlaySound(LinceAudioManager* audio, const char* filename);

// void LinceStreamSound(LinceAudioManager* audio, const char* filename);

// void LinceLoadSound(LinceAudioManager* audio, const char* name, const char* filename);

#endif /* LINCE_AUDIO_H */