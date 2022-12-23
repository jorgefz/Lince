#include <stdio.h>
#include <lince.h>
#include "audio.h"

#define TIMER_MAX 3000.0f

const char* audio_file = "sandbox/assets/cat.wav";
const char* music_file = "sandbox/assets/game-town-music.wav";

LinceSoundCollection* sound_bundle = NULL;
LinceSound music = {
    .type = LinceSound_Stream,
    .filename = "sandbox/assets/game-town-music.wav",
    .volume = 0.5f
};


LinceAudioManager audio;


void DrawUI(){

    LinceUILayer* ui = LinceGetAppState()->ui;
    struct nk_context *ctx = ui->ctx;
    
    nk_style_set_font(ui->ctx, &ui->fonts[LinceFont_Droid15]->handle);
    if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE
    )) {
        nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "Start")){
            LincePlaySoundInstance(&music);
        }
        if (nk_button_label(ctx, "Stop")){
            LinceStopSoundInstance(&music);
        }
        if (nk_button_label(ctx, "Reset")){
            LinceRewindSoundInstance(&music);
        }

        nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "Meow")){
            LinceSound config = {.volume = 0.1f};
            LinceSpawnSound(&audio, sound_bundle, &config);
        }

        nk_layout_row_dynamic(ctx,30,2);
        nk_slider_float(ctx, 0.0f, &music.volume, 5.0f, 0.1f);
    } 
    nk_end(ctx);
    
    LinceUpdateSoundInstance(&music);
}

void OnUpdate(float dt){
    DrawUI();
}

int main(int argc, char** argv) {

    LinceApp* app = LinceGetAppState();
    app->game_on_update = OnUpdate;

    LinceInitAudioManager(&audio);
    sound_bundle = LinceInitSoundCollection(LinceSound_Buffer, audio_file);
    LinceCreateSoundInstance(&audio, &music);
    LincePlaySoundInstance(&music);

    LinceRun();

    LinceDeleteSoundCollection(sound_bundle);
    LinceTerminateAudioManager(&audio);

    return 0;
}



#if 0

// ECS API

struct Sprite;
struct Collider;
struct Timer;

component_number = 3;
enum ComponentIDs { SPRITE_COMP, COLLIDER_COMP, TIMER_COMP };
LinceEntityRegistry* reg = LinceEntityRegistryInit(
    component_number,
    sizeof(Sprite),
    sizeof(Collider),
    sizeof(Timer)
);

Entity* e = LinceCreateEntity(reg);
ecs_add(e, SPRITE_COMP, LoadSprite("player.png"));

while (1) {
    // Draw entities that have 'Sprite' component
    LinceEntityQuery* query = LinceQueryEntities(2, SPRITE_COMP, COLLIDER_COMP);
    for(i in query->size){
        Sprite* sprite = LinceGetComp(query, i, SPRITE_COMP);
        Collider* collider =  LinceGetComp(query, i, COLLIDER_COMP);
        
        // systems
        DrawSprite(sprite);
    }

}



// ECS Implementation

struct ComponentStore{

    void* data; // raw contiguous array of components - 
}

struct Entity {

}

struct Registry {
    array_t(uint32_t)* component_sizes;
    array_t(uint32_t)* entity_ids;

    array_t* entity_pool; // ecs_create adds to this, 
}


#endif