#include <stdio.h>
#include <lince.h>
#include "audio.h"

#define TIMER_MAX 2000.0f

float counter_ms = TIMER_MAX; // milliseconds
const char* audio_file = "sandbox/assets/cat.wav";
const char* music_file = "sandbox/assets/game-town-music.wav";
LinceAudioManager audio;
ma_sound sound_stream;

/*

- Nuklear window to switch between menus
    Buttoms for each menu/layer

*/

void OnUpdate(float dt){
    counter_ms -= dt;
    if (counter_ms <= 0.0f){
        LincePlaySound(&audio, audio_file);
        counter_ms = TIMER_MAX;
    }
}

int main(int argc, char** argv) {

    LinceApp* app = LinceGetAppState();
    app->game_on_update = OnUpdate;

    LinceInitAudioManager(&audio);

    LinceStreamSound(&audio, music_file);

    LinceRun();

    LinceDeleteAudioManager(&audio);

    return 0;
}
