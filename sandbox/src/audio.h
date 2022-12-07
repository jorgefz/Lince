#ifndef LINCE_AUDIO_H
#define LINCE_AUDIO_H

#include <miniaudio.h>

typedef struct LinceAudioManager {
    ma_engine engine;
} LinceAudioManager;

void LinceInitAudioManager(LinceAudioManager* audio);

void LinceDeleteAudioManager(LinceAudioManager* audio);

void LincePlaySound(LinceAudioManager* audio, const char* filename);


#endif /* LINCE_AUDIO_H */