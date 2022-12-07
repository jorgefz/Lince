#include <stdio.h>
#include <lince.h>
#include "audio.h"

#define TIMER_MAX 1500.0f
float counter_ms = TIMER_MAX; // milliseconds
char* audio_file = NULL;
LinceAudioManager audio;

void OnUpdate(float dt){
    counter_ms -= dt;
    if (counter_ms <= 0.0f){
        LincePlaySound(&audio, audio_file);
        counter_ms = TIMER_MAX;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("No input file.\n");
        return -1;
    }

    audio_file = argv[1];
    
    LinceApp* app = LinceGetAppState();
    app->game_on_update = OnUpdate;

    LinceInitAudioManager(&audio);

    LinceRun();

    LinceDeleteAudioManager(&audio);

    return 0;
}
