#include <stdio.h>
#include <lince.h>
#include <lince/audio/audio.h>

const char* audio_file = "sandbox/assets/cat.wav";
const char* music_file = "sandbox/assets/game-town-music.wav";

LinceSoundManager* sound_manager = NULL;
LinceSound* music = NULL;
LinceAudioEngine* audio;

void DrawUI(){

    LinceUILayer* ui = LinceGetAppState()->ui;
    struct nk_context *ctx = ui->ctx;
    static LinceSoundConfig config = {.volume = 0.1f};
    
    nk_style_set_font(ui->ctx, &ui->fonts[LinceFont_Droid15]->handle);
    if (nk_begin(ctx, "Demo", nk_rect(20, 20, 300, 450),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE
    )) {
        nk_layout_row_static(ctx, 30, 80, 1);
        nk_label(ctx, "MUSIC", NK_TEXT_ALIGN_CENTERED);
        if (nk_button_label(ctx, "Start")){
            LincePlaySound(music);
        }
        if (nk_button_label(ctx, "Stop")){
            LinceStopSound(music);
        }
        if (nk_button_label(ctx, "Reset")){
            LinceRewindSound(music);
        }
        nk_label(ctx, "Volume", NK_TEXT_ALIGN_LEFT);
        nk_slider_float(ctx, 0.0f, &music->config.volume, 5.0f, 0.01f);
        
        nk_label(ctx, "Pitch", NK_TEXT_ALIGN_LEFT);
        nk_slider_float(ctx, 0.5f, &music->config.pitch, 2.5f, 0.01f);
        
        nk_label(ctx, "Pan", NK_TEXT_ALIGN_LEFT);
        nk_slider_float(ctx, -1.0f, &music->config.pan, 1.0f, 0.01f);

        nk_layout_row_static(ctx, 30, 80, 1);
        nk_label(ctx, "SOUNDS", NK_TEXT_ALIGN_CENTERED);
        nk_labelf(ctx, NK_TEXT_ALIGN_CENTERED, "Instances: %u", sound_manager->sound_cache.size);
        if (nk_button_label(ctx, "Meow")){
            LinceSpawnSound(audio, sound_manager, &config);
        }
        if (nk_button_label(ctx, "Stop All")){
            LinceStopAllManagerSounds(sound_manager);
        }
        nk_label(ctx, "Volume", NK_TEXT_ALIGN_LEFT);
        nk_slider_float(ctx, 0.0f, &config.volume, 2.0f, 0.01f);

    }
    nk_end(ctx);
    LinceUpdateSound(music);
}

void OnUpdate(float dt){
    DrawUI();
    LINCE_UNUSED(dt);
}

int main(void) {

    LinceApp* app = LinceGetAppState();
    app->game_on_update = OnUpdate;

    audio = LinceCreateAudioEngine();
    
    sound_manager = LinceCreateSoundManager(audio, LinceSound_Buffer, audio_file);
    
    LinceSoundConfig config = LinceGetDefaultSoundConfig();
    config.loop = LinceTrue;
    music = LinceLoadStream(audio, music_file, &config);
    LincePlaySound(music);

    LinceRun();

    LinceDeleteSound(music);
    LinceDeleteSoundManager(sound_manager);
    LinceDeleteAudioEngine(audio);

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
    for(int i = 0; i != query->size; ++i){
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