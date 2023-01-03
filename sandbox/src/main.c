#include <stdio.h>
#include <lince.h>
#include <lince/audio/audio.h>

#define LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT 2

#include "ecs.h"

void TestECS(){

    LinceEntityRegistry* reg;

    // These should cause an error in debug mode
    // reg = LinceCreateEntityRegistry(0); // error: zero components
    // reg = LinceCreateEntityRegistry(65); // error: too many components (max 64)
    // reg = LinceCreateEntityRegistry(3, /*sizes:*/ 1, 2, 0); // error: component is zero-sized

    struct Position { float x, y; };
    struct Velocity { float vx, vy; };
    struct Sprite   { LinceQuadProps data; };

    typedef enum {CompPosition, CompVelocity, CompSprite} Components;

    uint32_t comp_sizes[] = {sizeof(struct Position), sizeof(struct Velocity), sizeof(struct Sprite)};
    uint32_t comp_offsets[] = {0, comp_sizes[0], comp_sizes[0]+comp_sizes[1]}; 
    uint32_t comp_num = 3;

    // Create Registry
    reg = LinceCreateEntityRegistry(comp_num, comp_sizes[0], comp_sizes[1], comp_sizes[2]);

    LINCE_ASSERT(reg->component_count == comp_num && reg->component_sizes.size == comp_num,
        "Component count is not three");
    LINCE_ASSERT( comp_sizes[0] == *(uint32_t*)array_get(&reg->component_sizes, 0), "1st component size is wrong");
    LINCE_ASSERT( comp_sizes[1] == *(uint32_t*)array_get(&reg->component_sizes, 1), "2nd component size is wrong");
    LINCE_ASSERT( comp_sizes[2] == *(uint32_t*)array_get(&reg->component_sizes, 2), "3rd component size is wrong");

    LINCE_ASSERT( comp_offsets[0] == *(uint32_t*)array_get(&reg->component_offsets, 0), "1st component offset is wrong");
    LINCE_ASSERT( comp_offsets[1] == *(uint32_t*)array_get(&reg->component_offsets, 1), "2nd component offset is wrong");
    LINCE_ASSERT( comp_offsets[2] == *(uint32_t*)array_get(&reg->component_offsets, 2), "3rd component offset is wrong");

    // Create Entity
    uint32_t id = LinceCreateEntity(reg);
    LINCE_ASSERT(
        id == 0 && reg->entity_count == 1
        && reg->entity_masks.size == 1 && reg->entity_flags.size == 1
        && reg->entity_data.size == 1,
        "Failed to create entity"
    );
    uint64_t* mask = array_get(&reg->entity_masks, id);
    LinceEntityState* flag = array_get(&reg->entity_flags, id);
    LINCE_ASSERT(*flag & LinceEntityState_Active, "Failed to setup entity flags");
    LINCE_ASSERT(*mask == 0, "Failed to setup entity mask");

    // Create second entity
    uint32_t id2 = LinceCreateEntity(reg);
    LINCE_ASSERT(
        id2 == 1 
        && reg->entity_count == 2      && reg->entity_masks.size == 2
        && reg->entity_flags.size == 2 && reg->entity_data.size == 2,
        "Failed to create second entity"
    );

    // Delete entity
    LinceDeleteEntity(reg, id);
    LINCE_ASSERT(
        *flag == 0 && *mask == 0 && reg->entity_pool.size == 1
        && *(uint32_t*)array_get(&reg->entity_pool, 0) == id,
        "Failed to delete entity"
    );

    // Create third entity - should recycle first entity (with id 0)
    uint32_t id3 = LinceCreateEntity(reg);
    LINCE_ASSERT(
        id3 == id
        && reg->entity_count == 2      && reg->entity_masks.size == 2 
        && reg->entity_flags.size == 2 && reg->entity_data.size == 2,
        "Failed to create third entity"
    );

    // Add components to entity id = 0
    LinceAddEntityComponent(reg, id3, CompPosition, &(struct Position){1.0, 2.0});
    LinceAddEntityComponent(reg, id3, CompSprite,   &(struct Sprite){0});

    LINCE_ASSERT(LinceHasEntityComponent(reg, id3, CompPosition),
        "Failed to add component %d to entity %u", CompPosition, id3);
    LINCE_ASSERT(LinceHasEntityComponent(reg, id3, CompSprite),
        "Failed to add component %d to entity %u", CompSprite, id3);
    LINCE_ASSERT(LinceHasEntityComponent(reg, id3, CompVelocity) == LinceFalse,
        "Added wrong component %d to entity %u", CompVelocity, id3);

    struct Position* pos = LinceGetEntityComponent(reg, id3, CompPosition);
    LINCE_ASSERT(pos && pos->x==1.0 && pos->y==2.0, "Failed to retrieve position component");
    struct Velocity* vel = LinceGetEntityComponent(reg, id3, CompVelocity);
    LINCE_ASSERT(!vel, "Retrieved uninitialised velocity component");
    struct Sprite* sprite = LinceGetEntityComponent(reg, id3, CompSprite);
    LINCE_ASSERT(sprite, "Failed to retrieve sprite component");

    // Add components to entity id = 1
    LinceAddEntityComponent(reg, id2, CompPosition, &(struct Position){1.0, 2.0});
    LinceAddEntityComponent(reg, id2, CompVelocity, &(struct Velocity){-1.0, 5.0});

    LINCE_ASSERT(LinceHasEntityComponent(reg, id2, CompPosition),
        "Failed to add component %d to entity %u", CompPosition, id3);
    LINCE_ASSERT(LinceHasEntityComponent(reg, id2, CompVelocity),
        "Failed to add component %d to entity %u", CompVelocity, id3);
    LINCE_ASSERT(LinceHasEntityComponent(reg, id2, CompSprite) == LinceFalse,
        "Added wrong component %d to entity %u", CompSprite, id3);

    pos = LinceGetEntityComponent(reg, id2, CompPosition);
    LINCE_ASSERT(pos && pos->x==1.0 && pos->y==2.0, "Failed to retrieve position component");
    sprite = LinceGetEntityComponent(reg, id2, CompSprite);
    LINCE_ASSERT(!sprite, "Retrieved uninitialised sprite component");
    vel = LinceGetEntityComponent(reg, id2, CompVelocity);
    LINCE_ASSERT(vel, "Failed to retrieve velocity component");
    
    // Delete components
    LinceDeleteEntityComponent(reg, id2, CompVelocity);
    LINCE_ASSERT(LinceHasEntityComponent(reg, id2, CompVelocity) == LinceFalse,
        "Failed to delete component %d from entity %u", CompVelocity, id2);

    // Query entities
    // -- query sprite component
    array_t query_result_sprite;
    array_init(&query_result_sprite, sizeof(uint32_t));
    uint32_t count_sprite = LinceQueryEntities(reg, &query_result_sprite, 1, CompSprite);
    LINCE_ASSERT(
        count_sprite == 1 && query_result_sprite.size == 1
        && *(uint32_t*)array_get(&query_result_sprite, 0) == id3,
        "Failed to query entities with Sprite component"
    );
    array_uninit(&query_result_sprite);
    
    // -- query position component
    array_t query_result_pos;
    array_init(&query_result_pos, sizeof(uint32_t));
    uint32_t count_pos = LinceQueryEntities(reg, &query_result_pos, 1, CompPosition);
    LINCE_ASSERT(
        count_pos == 2 && query_result_pos.size == 2
        && *(uint32_t*)array_get(&query_result_pos, 0) == id3
        && *(uint32_t*)array_get(&query_result_pos, 1) == id2,
        "Failed to query entities with Position component"
    );
    array_uninit(&query_result_pos);
    
    // -- query sprite and position components
    array_t query_result_sprite_vel;
    array_init(&query_result_sprite_vel, sizeof(uint32_t));
    uint32_t count_sprite_vel = LinceQueryEntities(reg, &query_result_sprite_vel, 2, CompSprite, CompPosition);
    LINCE_ASSERT(
        count_sprite_vel == 1 && query_result_sprite_vel.size == 1,
        "Failed to query entities with Sprite and Position components"
    );
    array_uninit(&query_result_sprite_vel);

    // -- query velocity and position components
    array_t query_result_pos_vel;
    array_init(&query_result_pos_vel, sizeof(uint32_t));
    uint32_t count_pos_vel = LinceQueryEntities(reg, &query_result_pos_vel, 2, CompVelocity, CompPosition);
    LINCE_ASSERT(
        count_pos_vel == 0 && query_result_pos_vel.size == 0,
        "Failed to query entities with Velocity and Position components"
    );
    array_uninit(&query_result_pos_vel);

    // Destroy
    LinceDestroyEntityRegistry(reg);

    exit(0);
}

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

    TestECS();

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