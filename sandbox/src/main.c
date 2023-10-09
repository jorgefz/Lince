#include <stdio.h>
#include <time.h>

#define LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT 2

#include <lince.h>
#include <lince/audio/audio.h>
#include <lince/physics/boxcollider.h>

#include "gamedata.h"

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


#define MOVERS_SIZE 0.1f
#define MOVERS_COUNT 0

// WALKING
enum WalkingAnims {
    ANIM_FRONT = 0,
    ANIM_BACK,
    ANIM_LEFT,
    ANIM_RIGHT,
    ANIM_FRONT_IDLE,
    ANIM_BACK_IDLE,
    ANIM_LEFT_IDLE,
    ANIM_RIGHT_IDLE,
    ANIM_COUNT
};

typedef struct OldGameData {
    // Audio
    LinceSoundManager* sound_manager;
    LinceSound* music;
    LinceAudioEngine* audio;
    const char* audio_file;
    const char* music_file;

    // Rendering
    LinceCamera camera;
    LinceShader* custom_shader;

    // Entities
    LinceEntityRegistry* reg;
    uint32_t player;

    // Tile animation test
    uint8_t current_anim;
    uint32_t player_anim_order[ANIM_COUNT*2];
    LinceTileAnim* player_anim;
    LinceTexture* walking_tileset;
    LinceTile* tiles;
    size_t tile_count;

    // Tilemap test
    LinceTilemap mapgrid;
    LinceTilemap citygrid;
    
} OldGameData;

static OldGameData game_data = {
    .audio_file = "sandbox/assets/sounds/cat.wav",
    .music_file = "sandbox/assets/sounds/game-town-music.wav"
};


void LinceTransformToScreen(mat4 vp, vec2 world_pos, vec2 screen_pos){
	float wx = world_pos[0], wy = world_pos[1];

    // Transform by VP matrix
    vec4 wpos = {wx, wy, 0.0, 1.0};
    vec4 spos;
    glm_mat4_mulv(vp, wpos, spos);

    // Normalise from NDC to clip space
    screen_pos[0] = (spos[0]/spos[3]+1.0)/2.0;
    screen_pos[1] = (spos[1]/spos[3]+1.0)/2.0;
}


void AnimateWalking(float dt){

    // camera & player movement
    uint32_t next_anim = game_data.current_anim;

    enum {IDLE=0x0, UP=0x1, DOWN=0x2, LEFT=0x4, RIGHT=0x8};
    uint8_t direction = IDLE;
    direction |= LinceIsKeyPressed(LinceKey_w) * UP;
    direction |= LinceIsKeyPressed(LinceKey_s) * DOWN;
    direction |= LinceIsKeyPressed(LinceKey_a) * LEFT;
    direction |= LinceIsKeyPressed(LinceKey_d) * RIGHT;

    const float dr = dt * 1e-3;
    vec2 next_pos = {game_data.camera.pos[0], game_data.camera.pos[1]};

    if (direction & UP){
        next_pos[1] += dr;
        next_anim = ANIM_FRONT;
    }
    if (direction & DOWN){
        next_pos[1] -= dr;
        next_anim = ANIM_BACK;
    }
    if (direction & RIGHT){
        next_pos[0] += dr;
        next_anim = ANIM_RIGHT;
    }
    if (direction & LEFT){
        next_pos[0] -= dr;
        next_anim = ANIM_LEFT;
    }
    if(direction == IDLE){
        switch(game_data.current_anim){
        case ANIM_FRONT: next_anim = ANIM_FRONT_IDLE; break;
        case ANIM_BACK:  next_anim = ANIM_BACK_IDLE;  break;
        case ANIM_RIGHT: next_anim = ANIM_RIGHT_IDLE; break;
        case ANIM_LEFT:  next_anim = ANIM_LEFT_IDLE;  break;
        default: break;
        };
    }

    game_data.player_anim->order[0] = game_data.player_anim_order[next_anim*2];
    game_data.player_anim->order[1] = game_data.player_anim_order[next_anim*2+1];

    if(next_anim != game_data.current_anim){        
        LinceResetTileAnim(game_data.player_anim);
        game_data.current_anim = next_anim;
    }

}

void SetupTileAnimData(){
    game_data.walking_tileset = LinceLoadTexture(
        "sandbox/assets/textures/elv-games-movement.png", 0);

    // Set up texture coordinates for walking tile animation
    vec2 walk_texsize = {(float)game_data.walking_tileset->width,
                        (float)game_data.walking_tileset->height};
    vec2 walk_tilesize = {1,1};
    vec2 walk_cellsize = {24,24};
    vec2 walk_tile_locs[] = {
        {0,0}, {2,0}, // Walking forward
        {0,3}, {2,3}, // Walking backward
        {0,2}, {2,2}, // Walking left
        {0,1}, {2,1}, // Walking right
        {1,0}, // Idle forward
        {1,3}, // Idle backward
        {1,2}, // Idle left
        {1,1}, // Idle right
    };
    LinceTile walk_tiles[sizeof(walk_tile_locs)/sizeof(vec2)];
    for(uint32_t i = 0; i != sizeof(walk_tile_locs)/sizeof(vec2); ++i){
        LinceGetTileCoords(&walk_tiles[i], walk_texsize, walk_tile_locs[i],
            walk_cellsize, walk_tilesize);
    }
    uint32_t order_indices[] = {
        0,  1,   // walk forward
        2,  3,   // walk backwards
        4,  5,   // walk left
        6,  7,   // walk right
        8,  8,   // idle forward
        9,  9,   // idle backward
        10, 10,  // idle left
        11, 11   // idle right
    };
    memmove(game_data.player_anim_order, order_indices, sizeof(uint32_t)*ANIM_COUNT*2);
    game_data.current_anim = ANIM_FRONT_IDLE;
    /*
    game_data.player_anim = LinceCreateTileAnim(&(LinceTileAnim){
        .frames = walk_tiles,
        .frame_count = sizeof(walk_tiles)/sizeof(LinceTile),
        .frame_time = 300.0f,
        .order = order_indices + ANIM_FRONT_IDLE*2,
        .order_count = 2
    });
    */
}

void SetupChickenAnimation(){
    const char* fname = "sandbox/assets/textures/chicken.png";
    array_t chicken_tiles;
    vec2 cellsize = {16,16};
    LinceTexture* chicken_tileset = LinceLoadTextureWithTiles(fname, cellsize, &chicken_tiles);

    uint32_t chicken_id = LinceCreateEntity(game_data.reg);
    LinceSprite chicken_sprite = {
        .x=0.0, .y=0.5, .w=0.2, .h=0.2,
        .color={1,1,1,1},
        .texture = chicken_tileset
    };
    LinceBoxCollider chicken_box = {
        .x=chicken_sprite.x, .y=chicken_sprite.y,
        .w=chicken_sprite.w, .h=chicken_sprite.h,
        .dx=0.0, .dy=0.0, .flags = LinceBoxCollider_Bounce};
    LinceAddEntityComponent(game_data.reg, chicken_id, Component_Sprite, &chicken_sprite);
    LinceAddEntityComponent(game_data.reg, chicken_id, Component_BoxCollider, &chicken_box);

    LinceTileAnim chicken_anim = {
        .frames = &chicken_tiles,
        .frame_time = 400.0f, // ms
        .on_repeat = NULL, // ChickenLoops,
        .on_finish = NULL, // ChickenEnds,
        .repeats = 0, // forever
        .start = 1,
        // .order = (uint32_t[]){0,1,4,5},
        // .order_count = 4
    };
    LinceCreateTileAnim(&chicken_anim);
    LinceAddEntityComponent(game_data.reg, chicken_id,
        Component_TileAnim, &chicken_anim);
    array_uninit(&chicken_tiles);
}

void UpdateTileAnimations(float dt){
    
    LinceSprite* sprite;
    LinceTileAnim* anim;
    array_t query;
    array_init(&query, sizeof(uint32_t));
    LinceQueryEntities(game_data.reg, &query, 2, Component_TileAnim, Component_Sprite);
    
    for (uint32_t i = 0; i != query.size; ++i){
        uint32_t id = *(uint32_t*)array_get(&query, i);
        sprite = LinceGetEntityComponent(game_data.reg, id, Component_Sprite);
        anim   = LinceGetEntityComponent(game_data.reg, id, Component_TileAnim);
        LinceUpdateTileAnim(anim, dt);
        sprite->tile = anim->current_tile;
    }
    array_uninit(&query);

}

void LinceDrawSprites(LinceEntityRegistry* reg){

    // Setup lightning shader uniforms
    LinceBindShader(game_data.custom_shader);
    vec2 wsize;
    LinceGetScreenSize(wsize);
    LinceSetShaderUniformVec2(game_data.custom_shader, "uWindowSize", wsize);
    vec2 lightpos;
    LinceGetMousePos(&lightpos[0], &lightpos[1]);
    // convert to proper coords (top left in pixels, to bottom left [0,1])
    lightpos[0] = lightpos[0]/wsize[0];
    lightpos[1] = (1.0 - lightpos[1])/wsize[1] + 1.0;
    LinceSetShaderUniformVec2(game_data.custom_shader, "uPointLightPositions[0]", lightpos);
    
    LinceSprite* psprite = LinceGetEntityComponent(game_data.reg, game_data.player, Component_Sprite);
    vec2 player_pos = {psprite->x,psprite->y};
    LinceTransformToScreen(game_data.camera.view_proj, player_pos, player_pos);

    LinceSetShaderUniformVec2(game_data.custom_shader, "uPointLightPositions[1]", player_pos);
    LinceSetShaderUniformFloat(game_data.custom_shader, "uPointLightCount", 2.0);

    // Draw all entities
    static array_t result;
    array_init(&result, sizeof(uint32_t));
    uint32_t num = LinceQueryEntities(reg, &result, 1, Component_Sprite);

    for(uint32_t i = 0; i != num; ++i){
        uint32_t id = *(uint32_t*)array_get(&result, i);
        LinceSprite* sprite = LinceGetEntityComponent(reg, id, Component_Sprite);
        // LinceShader* shader = LinceGetEntityComponent(reg, id, Component_Shader);
        // BindUniformBuffer(...);
        LinceDrawSprite(sprite, game_data.custom_shader);
    }

    // You need to start a new batch in order to change the value of an uniform
    // Otherwise, changing the uniform will overwrite the old value.
    // Consider using uniform buffer objects or SSBOs
    // LinceStartNewBatch();

    // Draw test quad at origin
    // LinceDrawSprite(&(LinceSprite){
    //     .x = 0.0, .y = 0.0,
    //     .w = 3.0, .h = 2.0,
    //     .color = {0.1,0.1,0.1,1}
    // }, game_data.custom_shader);

    array_uninit(&result);
}

static const float vel = 8e-4;


void UpdateSpritePositions(LinceEntityRegistry* reg){
    static array_t result;
    array_init(&result, sizeof(uint32_t));
    uint32_t num = LinceQueryEntities(reg, &result, 2, Component_Sprite, Component_BoxCollider);

    for(uint32_t i = 0; i != num; ++i){
        uint32_t id = *(uint32_t*)array_get(&result, i);
        LinceSprite* sprite = LinceGetEntityComponent(game_data.reg, id, Component_Sprite);
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

    // Update camera position to follow player
    game_data.camera.pos[0] = pbox->x;
    game_data.camera.pos[1] = pbox->y;

}

void GameStateInit(){

    // Resources
    LinceInitCamera(&game_data.camera, LinceGetAspectRatio());
    game_data.camera.zoom = 1.3f;

    game_data.custom_shader = LinceCreateShader(
		"sandbox/assets/shaders/light.vert.glsl",
		"sandbox/assets/shaders/light.frag.glsl"
    );
    LinceBindShader(game_data.custom_shader);
    #define max_texture_slots 32
	int samplers[max_texture_slots] = { 0 };
	for (int i = 0; i != max_texture_slots; ++i) samplers[i] = i;
	LinceSetShaderUniformIntN(game_data.custom_shader, "uTextureSlots", samplers, max_texture_slots);

    // Entities
    game_data.reg = LinceCreateEntityRegistry(
        Component_Count, COMPONENT_SIZES
    );

    // --> walls
    /*
    uint32_t walls[4];
    LinceBoxCollider wall_boxes[4] = {
        {.x =  0.0f, .y =  1.0f, .w =  2.0f, .h = 0.01f},
        {.x = -1.0f, .y =  0.0f, .w = 0.01f, .h =  2.0f},
        {.x =  0.0f, .y = -1.0f, .w =  2.0f, .h = 0.01f},
        {.x =  1.0f, .y =  0.0f, .w = 0.01f, .h =  2.0f},
    };
    for(uint32_t i = 0; i != 4; ++i){
        walls[i] = LinceCreateEntity(game_data.reg);
        LinceAddEntityComponent(game_data.reg, walls[i], Component_BoxCollider, &wall_boxes[i]);
        LinceSprite wall_sprite = {.x=wall_boxes[i].x, .y=wall_boxes[i].y,
            .w=wall_boxes[i].w, .h=wall_boxes[i].h, .color={0,1,1,1}};
        LinceAddEntityComponent(game_data.reg, walls[i], Component_Sprite, &wall_sprite);   
    }
    */

    // --> player
    LinceSprite sprite = {
        .x = 0.0, .y = 0.0,
        .w = 0.1, .h = 0.1,
        .color = {0.0, 0.0, 1.0, 1.0}
    };
    LinceBoxCollider box = {.x=sprite.x, .y=sprite.y, .w=sprite.w, .h=sprite.h, .dx=0, .dy=0 };
    game_data.player = LinceCreateEntity(game_data.reg);
    LinceAddEntityComponent(game_data.reg, game_data.player, Component_Sprite, &sprite);
    LinceAddEntityComponent(game_data.reg, game_data.player, Component_BoxCollider, &box);

    // --> static blocks
    /*
    sprite.color[0] = 1.0;
    sprite.color[2] = 0.0;
    sprite.w *= 4.0f;
    int obstacle_count = 4;
    float pos_x[] = {0.6,  0.8, -0.6, -0.6};
    float pos_y[] = {0.6,  0.4, -0.6,  0.6};
    for(int i = 0; i != obstacle_count; ++i){
        uint32_t wall_entity = LinceCreateEntity(game_data.reg);
        sprite.x = pos_x[i];
        sprite.y = pos_y[i];
        LinceBoxCollider obox = {
            .x=sprite.x, .y=sprite.y,
            .w=sprite.w, .h=sprite.h,
            .dx=0, .dy=0,
            .flags = LinceBoxCollider_Static
        };
        LinceAddEntityComponent(game_data.reg, wall_entity, Component_Sprite, &sprite);
        LinceAddEntityComponent(game_data.reg, wall_entity, Component_BoxCollider, &obox);
    }
    */

    // --> movers
    srand(time(NULL));
    sprite.color[0] = 0.0;
    sprite.color[1] = 1.0;
    sprite.w = MOVERS_SIZE;
    sprite.h = MOVERS_SIZE;
    for(int i = 0; i != MOVERS_COUNT; ++i){
        uint32_t mover_id = LinceCreateEntity(game_data.reg);
        float min = -1.0f, max = 1.0f;
        sprite.x = min + rand()/(float)RAND_MAX * (max - min);
        sprite.y = min + rand()/(float)RAND_MAX * (max - min);
        min = -8e-4;
        max =  8e-4;
        float dx = min + rand()/(float)RAND_MAX * (max - min);
        float dy = min + rand()/(float)RAND_MAX * (max - min);
        LinceBoxCollider mbox = {
            .x=sprite.x, .y=sprite.y,
            .w=sprite.w, .h=sprite.h,
            .dx=dx, .dy=dy,
            .flags = LinceBoxCollider_Bounce };
        LinceAddEntityComponent(game_data.reg, mover_id, Component_Sprite, &sprite);
        LinceAddEntityComponent(game_data.reg, mover_id, Component_BoxCollider, &mbox);
    }

    // Tile animation test
    SetupChickenAnimation();

    game_data.mapgrid = (LinceTilemap){
        .texture = LinceLoadTexture(
            "sandbox/assets/textures/hilau_tilesets/6_floors.png", 0
        ),
        .cellsize = {32,32},
        .scale = {0.4,0.4},
        .offset = {-1, -1},
        .width = 5,
        .height = 5,
        .grid = (uint32_t[]){
            10,10,10,10,10,
            10,10,10,10,10,
            10,10,10,10,10,
            10,10,10,10,10,
            10,10,10,10,10,
        }
    };
    LinceInitTilemap(&game_data.mapgrid);

    game_data.citygrid = (LinceTilemap){
        .texture = LinceLoadTexture(
            "sandbox/assets/textures/bricks.png", 0),
        .cellsize = {32,32},
        .scale = {0.4,0.4},
        .offset = {-1, -1},
        .width = 8,
        .height = 8,
        .grid = (uint32_t[]){
           66,66,66,66,66,66,66,66,
           66,33,29,29,29,29,34,66,
           66,30,36,36,43,43,31,66,
           66,30,11,11,14,14,31,66,
           66,30, 9, 9,12,12,31,66,
           66,32,28,28,28,28,35,66,
           66,66,66,66,66,66,66,66,
           66,66,66,66,66,66,66,66,
        }
    };
    LinceInitTilemap(&game_data.citygrid);
}

void GameStateUpdate(float dt){

    // Rendering
    LinceResizeCameraView(&game_data.camera, LinceGetAspectRatio());
	LinceUpdateCamera(&game_data.camera);

    // Tile animation test
    UpdateTileAnimations(dt);

    LinceBeginScene(&game_data.camera);
    LinceBindShader(game_data.custom_shader);
    LinceSetShaderUniformMat4(game_data.custom_shader,
        "u_view_proj", game_data.camera.view_proj);
    UpdateSpritePositions(game_data.reg);
    LinceDrawSprites(game_data.reg);

    // LinceDrawTilemap(&game_data.mapgrid, game_data.custom_shader);
    LinceDrawTilemap(&game_data.citygrid, game_data.custom_shader);

    // TEST TRANSLUCENCY
    LinceDrawSprite(&(LinceSprite){
        .x = -2, .y = -1,
        .w = 0.5, .h = 0.5,
        .color = {1, 0, 0, 0.4},
        .zorder = 0.2
    }, game_data.custom_shader);
    LinceDrawSprite(&(LinceSprite){
        .x = -2.1, .y = -1.1,
        .w = 0.5, .h = 0.5,
        .color = {1, 1, 0, 0.5},
        .zorder = -0.1
    }, game_data.custom_shader);

    LinceEndScene();

    // Move
    MovePlayer(dt);

    array_t entities;
    array_init(&entities, sizeof(uint32_t));
    LinceQueryEntities(game_data.reg, &entities, 1, Component_BoxCollider);
    LinceCalculateEntityCollisions(game_data.reg, &entities, Component_BoxCollider);
    array_uninit(&entities);

    // Draw UI text
    LinceUILayer* ui = LinceGetApp()->ui;
    struct nk_context *ctx = ui->ctx;
    nk_style_set_font(ctx, &ui->fonts[LinceFont_Droid15]->handle);
    if (nk_begin(ctx, "Demo", nk_rect(20, 20, 100, 100), 0)) {
        nk_layout_row_static(ctx, 25, 80, 1);
        nk_labelf(
            ctx, NK_TEXT_ALIGN_CENTERED,
            "FPS: %.1f", 1000.0f/dt
        );
        vec2 mouse;
        LinceGetMousePosWorld(mouse, &game_data.camera);
        nk_labelf(
            ctx, NK_TEXT_ALIGN_CENTERED,
            "%.2f %.2f", mouse[0], mouse[1]
        );
    }
    nk_end(ctx);
}

void GameTerminate(){

    // Delete textures and tile animations
    array_t query;
    array_init(&query, sizeof(uint32_t));

    // Delete textures on entities
    LinceSprite* s;
    LinceQueryEntities(game_data.reg, &query, 1, Component_Sprite);
    for(uint32_t i = 0; i != query.size; ++i){
        uint32_t id = *(uint32_t*)array_get(&query, i);
        s = LinceGetEntityComponent(game_data.reg, id, Component_Sprite);
        LINCE_ASSERT(s, "NULL tex");
        if(s->texture){
            LinceDeleteTexture(s->texture);
            s->texture = NULL;
        }
    }
    
    array_clear(&query);
    LinceTileAnim* anim;
    LinceQueryEntities(game_data.reg, &query, 1, Component_TileAnim);

    for(uint32_t i = 0; i != query.size; ++i){
        uint32_t id = *(uint32_t*)array_get(&query, i);
        anim = LinceGetEntityComponent(game_data.reg, id, Component_TileAnim);
        LinceDeleteTileAnim(anim);
    }
    
    array_uninit(&query);

    LinceUninitTilemap(&game_data.mapgrid);
    LinceUninitTilemap(&game_data.citygrid);

    LinceDestroyEntityRegistry(game_data.reg);
    LinceDeleteShader(game_data.custom_shader);
    
    LinceDeleteSound(game_data.music);
    LinceDeleteSoundManager(game_data.sound_manager);
    LinceDeleteAudioEngine(game_data.audio);
}


void DrawSoundUI(){

    LinceUILayer* ui = LinceGetApp()->ui;
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

    GameStateInit();
}

void GameOnUpdate(float dt){
    LinceCheckErrors();
    GameStateUpdate(dt);
    // DrawSoundUI();
}



/*
****************************************************
****************************************************
****************************************************
****************************************************
*/


#include "gamedata.h"
#include "scenes/scenes.h"

static GameData DATA = {0};

static const char* asset_dir = "../../../sandbox/assets2";
static const char* asset_name = "shaders/light.frag.glsl";    


void SandboxInit() {

    LincePushAssetDir(asset_dir);

    char asset_path[LINCE_PATH_MAX];
    LinceBool success = LinceFetchAssetPath(asset_path, "fonts/Cousine-Regular.ttf");
    printf("Asset %s\n", success?"found":"not found");
    printf("%20s\n", asset_path);

    LinceInitCamera(&DATA.camera, LinceGetAspectRatio());
    DATA.camera.zoom = 3.0f;
    DATA.camera_speed = 0.003f;

    DATA.player_box = (LinceBoxCollider){.x=0, .y=0, .w=0.7, .h=0.7};
    DATA.player_sprite = (LinceSprite){.x=0, .y=0, .w=0.7, .h=0.7, .color={0,0,1,1}, .zorder=1};
    
    hashmap_init(&DATA.scene_cache, 11);
    hashmap_set(&DATA.scene_cache, "MainMenu", &SCENE_CALLBACKS[Scene_MainMenu]);
    hashmap_set(&DATA.scene_cache, "World", &SCENE_CALLBACKS[Scene_World]);
    hashmap_set(&DATA.scene_cache, "House", &SCENE_CALLBACKS[Scene_House]);
    LinceGetApp()->user_data = &DATA;

    LincePushScene(hashmap_get(&DATA.scene_cache, "MainMenu"));
}

void SandboxUpdate(float dt){
    LinceResizeCameraView(&DATA.camera, LinceGetAspectRatio());
    LinceUpdateCamera(&DATA.camera);
}

void SandboxTerminate(){
    hashmap_uninit(&DATA.scene_cache);
}

void SetupApplication(){
    LinceApp* app = LinceGetApp();
    app->screen_width = 1280;
    app->screen_height = 720;
    app->on_init      = SandboxInit;
    app->on_update    = SandboxUpdate;
    app->on_terminate = SandboxTerminate;
    
}


int main(void) {

    SetupApplication();
    LinceRun();

    return 0;
}

