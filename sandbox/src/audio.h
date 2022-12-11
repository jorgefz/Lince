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

void LinceStreamSound(LinceAudioManager* audio, const char* filename);

// void LinceLoadSound(LinceAudioManager* audio, const char* name, const char* filename);

#endif /* LINCE_AUDIO_H */


/*========== API Design =========*/
#if 0

struct LinceSound {
    ma_sound* handle;
    char* filename;
}

void LinceLoadSound(manager, name, filename, mode){
    // Create LinceSound object
    // Store filename
    // Create ma_sound from filename with given mode
    // Store ma_sound handle in LinceSound object.
    // Store LinceSound object in hashmap using given name as key
}

void LincePlaySound(manager, name){
    // Find key in hashmap and retrieve LinceSound object
    // ma_sound_start
}

void LinceStopSound(manager, name){
    // Find key in hashmap and retrieve LinceSound object
    // ma_sound_stop
}

void LinceRestartSound(manager, name){
    // Find key in hashmap and retrieve LinceSound object
    // ma_sound_seek_pc_frames(0)
    // ma_sound_start
}

#endif