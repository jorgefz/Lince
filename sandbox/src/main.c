#include <stdio.h>
#include <time.h>

#define LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT 2

#include <lince.h>
#include <lince/audio/audio.h>

#include "boxcollider.h"

#define QUADTREE_CHILDREN 4
#define QUADTREE_NODE_CAPACITY 16 // max objects in node before it splits into children
#define QUADTREE_MAX_DEPTH 8 // max node depth for quadtree

typedef struct quadtree_container {
    struct quadtree_container* parent;
    struct quadtree_container* children[QUADTREE_CHILDREN];
    vec2 key;
    array_t objects;
} quadtree_t;

/*
QueryEntities(registry, mode, result, ncomp, ...)

Change 1: mode.
    any  -> any of these components
    all  -> all of these comps
    none -> none of these comps

Change 2: results.
    Results array can provide list of entity IDs.
    The input query will run only on these entities.
*/ 

#define MOVERS_SIZE 0.01f
#define MOVERS_COUNT 100

typedef struct GameData {
    // Audio
    LinceSoundManager* sound_manager;
    LinceSound* music;
    LinceAudioEngine* audio;
    const char* audio_file;
    const char* music_file;

    // Rendering
    LinceCamera* camera;

    // Entities
    LinceEntityRegistry* reg;
    uint32_t player;
    uint32_t obstacles[4];
    uint32_t movers[MOVERS_COUNT];
    
} GameData;

static GameData game_data = {
    .audio_file = "sandbox/assets/cat.wav",
    .music_file = "sandbox/assets/game-town-music.wav"
};

typedef enum Component { Component_BoxCollider, Component_Sprite } Component;

typedef LinceQuadProps Sprite;

void LinceDrawSpriteComponents(LinceEntityRegistry* reg){
    static array_t result;
    array_init(&result, sizeof(uint32_t));
    uint32_t num = LinceQueryEntities(reg, &result, 1, Component_Sprite);

    for(uint32_t i = 0; i != num; ++i){
        uint32_t id = *(uint32_t*)array_get(&result, i);
        Sprite* sprite = LinceGetEntityComponent(reg, id, Component_Sprite);
        LinceDrawQuad(*sprite);
    }

    array_uninit(&result);
}

static const float vel = 8e-4;


void UpdateSpritePositions(LinceEntityRegistry* reg){
    static array_t result;
    array_init(&result, sizeof(uint32_t));
    uint32_t num = LinceQueryEntities(reg, &result, 2, Component_Sprite, Component_BoxCollider);

    for(uint32_t i = 0; i != num; ++i){
        uint32_t id = *(uint32_t*)array_get(&result, i);
        Sprite* sprite = LinceGetEntityComponent(game_data.reg, id, Component_Sprite);
        LinceBoxCollider* box = LinceGetEntityComponent(game_data.reg, id, Component_BoxCollider);
        sprite->x = box->x;
        sprite->y = box->y;
    }
    array_uninit(&result);
}

void MovePlayer(float dt){

    LinceBoxCollider* pbox;
    pbox = LinceGetEntityComponent(game_data.reg, game_data.player, Component_BoxCollider);
    pbox->dx = 0.0f;
    pbox->dy = 0.0f;
    if(LinceIsKeyPressed(LinceKey_d)) pbox->dx =  vel * dt;
    if(LinceIsKeyPressed(LinceKey_a)) pbox->dx = -vel * dt;
    if(LinceIsKeyPressed(LinceKey_w)) pbox->dy =  vel * dt;
    if(LinceIsKeyPressed(LinceKey_s)) pbox->dy = -vel * dt;

}

void LayerOnAttach(LinceLayer* layer){
    LINCE_UNUSED(layer);

    // Rendering
    game_data.camera = LinceCreateCamera(LinceGetAspectRatio());

    // Entities
    game_data.reg = LinceCreateEntityRegistry(2, sizeof(LinceBoxCollider), sizeof(Sprite));

    // -- walls
    uint32_t walls[4];
    LinceBoxCollider wall_boxes[4] = {
        {.x =  0.0f, .y =  1.0f, .w =  2.0f, .h = 0.01f, .dx = 0.0f, .dy = 0.0f},
        {.x = -1.0f, .y =  0.0f, .w = 0.01f, .h =  2.0f, .dx = 0.0f, .dy = 0.0f},
        {.x =  0.0f, .y = -1.0f, .w =  2.0f, .h = 0.01f, .dx = 0.0f, .dy = 0.0f},
        {.x =  1.0f, .y =  0.0f, .w = 0.01f, .h =  2.0f, .dx = 0.0f, .dy = 0.0f},
    };
    for(uint32_t i = 0; i != 4; ++i){
        walls[i] = LinceCreateEntity(game_data.reg);
        LinceAddEntityComponent(game_data.reg, walls[i], Component_BoxCollider, &wall_boxes[i]);
        Sprite wall_sprite = {.x=wall_boxes[i].x, .y=wall_boxes[i].y,
            .w=wall_boxes[i].w, .h=wall_boxes[i].h, .color={0,1,1,1}};
        LinceAddEntityComponent(game_data.reg, walls[i], Component_Sprite, &wall_sprite);   
    }

    // -- player
    Sprite sprite = (Sprite){
        .x = 0.0, .y = 0.0,
        .w = 0.1, .h = 0.1,
        .color = {0.0, 0.0, 1.0, 1.0}
    };
    LinceBoxCollider box = {.x=sprite.x, .y=sprite.y, .w=sprite.w, .h=sprite.h, .dx=0, .dy=0 };
    game_data.player = LinceCreateEntity(game_data.reg);
    LinceAddEntityComponent(game_data.reg, game_data.player, Component_Sprite, &sprite);
    LinceAddEntityComponent(game_data.reg, game_data.player, Component_BoxCollider, &box);

    // -- static blocks
    sprite.color[0] = 1.0;
    sprite.color[2] = 0.0;
    sprite.w *= 4.0f;
    float pos_x[] = {0.6,  0.8, -0.6, -0.6};
    float pos_y[] = {0.6,  0.4, -0.6,  0.6};
    for(int i = 0; i != 4; ++i){
        game_data.obstacles[i] = LinceCreateEntity(game_data.reg);
        sprite.x = pos_x[i];
        sprite.y = pos_y[i];
        LinceBoxCollider obox = {.x=sprite.x, .y=sprite.y, .w=sprite.w, .h=sprite.h, .dx=0, .dy=0};
        LinceAddEntityComponent(game_data.reg, game_data.obstacles[i], Component_Sprite, &sprite);
        LinceAddEntityComponent(game_data.reg, game_data.obstacles[i], Component_BoxCollider, &obox);
    }

    // -- movers
    srand(time(NULL));
    sprite.color[0] = 0.0;
    sprite.color[1] = 1.0;
    sprite.w = MOVERS_SIZE;
    sprite.h = MOVERS_SIZE;
    for(int i = 0; i != MOVERS_COUNT; ++i){
        game_data.movers[i] = LinceCreateEntity(game_data.reg);
        float min = -1.0f, max = 1.0f;
        sprite.x = min + rand()/(float)RAND_MAX * (max - min);
        sprite.y = min + rand()/(float)RAND_MAX * (max - min);
        min = -8e-4;
        max =  8e-4;
        float dx = min + rand()/(float)RAND_MAX * (max - min);
        float dy = min + rand()/(float)RAND_MAX * (max - min);
        LinceBoxCollider mbox = {.x=sprite.x, .y=sprite.y, .w=sprite.w, .h=sprite.h, .dx=dx, .dy=dy,
            .flags = LinceBoxCollider_Bounce };
        LinceAddEntityComponent(game_data.reg, game_data.movers[i], Component_Sprite, &sprite);
        LinceAddEntityComponent(game_data.reg, game_data.movers[i], Component_BoxCollider, &mbox);
    }
}

void LayerOnUpdate(LinceLayer* layer, float dt){
    LINCE_UNUSED(layer);

    // Rendering
    LinceResizeCameraView(game_data.camera, LinceGetAspectRatio());
	LinceUpdateCamera(game_data.camera);

    LinceBeginScene(game_data.camera);
    UpdateSpritePositions(game_data.reg);
    LinceDrawSpriteComponents(game_data.reg);
    LinceEndScene();

    // Move
    MovePlayer(dt);

    array_t entities;
    array_init(&entities, sizeof(uint32_t));
    LinceQueryEntities(game_data.reg, &entities, 1, Component_BoxCollider);
    LinceCalculateEntityCollisions(game_data.reg, &entities, Component_BoxCollider);
    array_uninit(&entities);

    // Draw UI text
    LinceUILayer* ui = LinceGetAppState()->ui;
    struct nk_context *ctx = ui->ctx;
    nk_style_set_font(ctx, &ui->fonts[LinceFont_Droid15]->handle);
    if (nk_begin(ctx, "Demo", nk_rect(20, 20, 100, 50), 0)) {
        nk_layout_row_static(ctx, 30, 40, 1);
        nk_labelf(
            ctx, NK_TEXT_ALIGN_CENTERED,
            "FPS: %.1f", 1000.0f/dt
        );
    }
    nk_end(ctx);
}

void LayerOnDetach(LinceLayer* layer){
    LINCE_UNUSED(layer);
    LinceDestroyEntityRegistry(game_data.reg);
    LinceDeleteCamera(game_data.camera);
}

LinceLayer* LayerInit(){
	LinceLayer* layer = LinceCreateLayer(NULL);

	layer->OnAttach = LayerOnAttach;
	layer->OnUpdate = LayerOnUpdate;
	layer->OnEvent  = NULL;
	layer->OnDetach = LayerOnDetach;
	layer->data = NULL;

	return layer;
}


void DrawSoundUI(){

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
            LincePlaySound(game_data.music);
        }
        if (nk_button_label(ctx, "Stop")){
            LinceStopSound(game_data.music);
        }
        if (nk_button_label(ctx, "Reset")){
            LinceRewindSound(game_data.music);
        }
        nk_label(ctx, "Volume", NK_TEXT_ALIGN_LEFT);
        nk_slider_float(ctx, 0.0f, &game_data.music->config.volume, 5.0f, 0.01f);
        
        nk_label(ctx, "Pitch", NK_TEXT_ALIGN_LEFT);
        nk_slider_float(ctx, 0.5f, &game_data.music->config.pitch, 2.5f, 0.01f);
        
        nk_label(ctx, "Pan", NK_TEXT_ALIGN_LEFT);
        nk_slider_float(ctx, -1.0f, &game_data.music->config.pan, 1.0f, 0.01f);

        nk_layout_row_static(ctx, 30, 80, 1);
        nk_label(ctx, "SOUNDS", NK_TEXT_ALIGN_CENTERED);
        nk_labelf(ctx, NK_TEXT_ALIGN_CENTERED, "Instances: %u", game_data.sound_manager->sound_cache.size);
        if (nk_button_label(ctx, "Meow")){
            LinceSpawnSound(game_data.audio, game_data.sound_manager, &config);
        }
        if (nk_button_label(ctx, "Stop All")){
            LinceStopAllManagerSounds(game_data.sound_manager);
        }
        nk_label(ctx, "Volume", NK_TEXT_ALIGN_LEFT);
        nk_slider_float(ctx, 0.0f, &config.volume, 2.0f, 0.01f);

    }
    nk_end(ctx);
    LinceUpdateSound(game_data.music);
}

void GameInit(){
    game_data.audio = LinceCreateAudioEngine();
    game_data.sound_manager = LinceCreateSoundManager(
        game_data.audio, LinceSound_Buffer, game_data.audio_file);

    LinceSoundConfig config = LinceGetDefaultSoundConfig();
    config.loop = LinceTrue;
    game_data.music = LinceLoadStream(game_data.audio, game_data.music_file, &config);

    LincePushLayer(LayerInit());
}

void GameTerminate(){
    LinceDeleteSound(game_data.music);
    LinceDeleteSoundManager(game_data.sound_manager);
    LinceDeleteAudioEngine(game_data.audio);
}

void GameOnUpdate(float dt){
    LinceCheckErrors();
    // DrawSoundUI();
    LINCE_UNUSED(dt);
}

void SetupApplication(){
    LinceApp* app = LinceGetAppState();
    
    app->screen_width = 900;
    app->screen_height = 600;
    app->title = "Sandbox";
    
    app->game_init = GameInit;
    app->game_on_update = GameOnUpdate;
    app->game_terminate = GameTerminate;
}

int main(void) {

    SetupApplication();

    LinceRun();

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