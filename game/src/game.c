
#include <time.h>

#include "lince.h"
#include "cglm/types.h"
#include "cglm/vec4.h"
#include "cglm/affine.h"

#include "gui/nuklear_flags.h"
#include "gui/nuklear.h"

#include "pong.h"
#include "missile_command.h"
#include "nk_test.h"
#include <assert.h>

#include "renderer/tileset.h"
#include "renderer/tile_anim.h"



enum TileNames {
    // Ground
    TILE_GRASS             = 0,
    TILE_DIRT              = 1,

    TILE_GRASS_LEFT        = 2,
    TILE_GRASS_RIGHT       = 3,
    TILE_GRASS_UPPER       = 4,
    TILE_GRASS_LOWER       = 5,

    TILE_GRASS_CORNER_LL   = 6,
    TILE_GRASS_CORNER_UL   = 7,
    TILE_GRASS_CORNER_LR   = 8,
    TILE_GRASS_CORNER_UR   = 9,

    TILE_GRASS_ICORNER_LL   = 10,
    TILE_GRASS_ICORNER_UL   = 11,
    TILE_GRASS_ICORNER_LR   = 12,
    TILE_GRASS_ICORNER_UR   = 13,
    // Entities
    TILE_TREE,
    TILE_CHICKEN,
    // Meta
    TILE_COUNT
};

const uint32_t tm_width = 16, tm_height = 10;
int tilemap[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    4, 4, 4, 4,11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 7,11, 0, 0, 0, 0,13, 4, 4, 4, 4, 4, 
    1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 3, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 3, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 2, 0, 0, 0, 13,9, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 2, 0, 0, 0, 3, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 6,10, 0,13, 4, 9, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 2, 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 6,10, 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 
};


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


void ChickenLoops(LinceTileAnim* anim, void* args){
    printf("Chicken repeats\n");
}

void ChickenEnds(LinceTileAnim* anim, void* args){
    printf("Chicken stopped moving!\n");
}



// TEST LAYER

typedef struct TestLayer {
    char name[LINCE_NAME_MAX];
    float red, vel;
    float dt;
    float cam_speed, color_step;

    LinceVertexArray* va;
    LinceVertexBuffer vb;
    LinceIndexBuffer ib;
    LinceShader* shader;
    LinceTexture* tex_front;
    LinceTexture* tex_back;
    LinceTexture* tileset;
    LinceTexture* walking_tileset;

    LinceTile tiles[TILE_COUNT];
    LinceTileAnim* player_anims[ANIM_COUNT];
    uint8_t current_anim;

    LinceTileAnim* chicken_anim;

    vec4 color;
    LinceCamera* cam;
} TestLayer;

void TestLayerOnAttach(LinceLayer* layer) {
    TestLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' attached", data->name);

    data->red = 0.0f;
    data->vel = 5e-4f;
    data->color_step = 0.003f;
    
    data->cam = LinceCreateCamera(LinceGetAspectRatio());
    data->cam_speed = 9e-4f;
    data->cam->zoom = 4.0;

    data->tex_front = LinceCreateTexture("PatrickF", "lince/assets/front.png");
    data->tex_back  = LinceCreateTexture("PatrickB", "lince/assets/back.png");
    data->tileset = LinceCreateTexture("Tileset", "game/assets/textures/shubibubi-cozy-farm.png");
    data->walking_tileset = LinceCreateTexture("Walking", "game/assets/textures/elv-games-movement.png");

    data->tiles[TILE_GRASS]   = LinceGetTile(data->tileset, (vec2){1,8}, (vec2){16,16}, (vec2){1, 1});
    data->tiles[TILE_DIRT]    = LinceGetTile(data->tileset, (vec2){5,9}, (vec2){16,16}, (vec2){1, 1});
    
    data->tiles[TILE_GRASS_LEFT ] = LinceGetTile(data->tileset, (vec2){0,8}, (vec2){16,16}, (vec2){1,1});
    data->tiles[TILE_GRASS_RIGHT] = LinceGetTile(data->tileset, (vec2){2,8}, (vec2){16,16}, (vec2){1,1});
    data->tiles[TILE_GRASS_UPPER] = LinceGetTile(data->tileset, (vec2){1,9}, (vec2){16,16}, (vec2){1,1});
    data->tiles[TILE_GRASS_LOWER] = LinceGetTile(data->tileset, (vec2){1,7}, (vec2){16,16}, (vec2){1,1});

    data->tiles[TILE_GRASS_CORNER_LL] = LinceGetTile(data->tileset, (vec2){0,7}, (vec2){16,16}, (vec2){1,1});
    data->tiles[TILE_GRASS_CORNER_UL] = LinceGetTile(data->tileset, (vec2){0,9}, (vec2){16,16}, (vec2){1,1});
    data->tiles[TILE_GRASS_CORNER_LR] = LinceGetTile(data->tileset, (vec2){2,7}, (vec2){16,16}, (vec2){1,1});
    data->tiles[TILE_GRASS_CORNER_UR] = LinceGetTile(data->tileset, (vec2){2,9}, (vec2){16,16}, (vec2){1,1});

    data->tiles[TILE_GRASS_ICORNER_LL] = LinceGetTile(data->tileset, (vec2){4,8}, (vec2){16,16}, (vec2){1,1});
    data->tiles[TILE_GRASS_ICORNER_UL] = LinceGetTile(data->tileset, (vec2){4,7}, (vec2){16,16}, (vec2){1,1});
    data->tiles[TILE_GRASS_ICORNER_LR] = LinceGetTile(data->tileset, (vec2){3,8}, (vec2){16,16}, (vec2){1,1});
    data->tiles[TILE_GRASS_ICORNER_UR] = LinceGetTile(data->tileset, (vec2){3,7}, (vec2){16,16}, (vec2){1,1});

    data->tiles[TILE_TREE]    = LinceGetTile(data->tileset, (vec2){9,5}, (vec2){16,16}, (vec2){2, 2});
    data->tiles[TILE_CHICKEN] = LinceGetTile(data->tileset, (vec2){0,1}, (vec2){16,16}, (vec2){1, 1});

    LinceTile player_tiles[] = {
        // Walking Forward
        LinceGetTile(data->walking_tileset, (vec2){0,0}, (vec2){24,24}, (vec2){1,1}),
        LinceGetTile(data->walking_tileset, (vec2){2,0}, (vec2){24,24}, (vec2){1,1}),
        // Walking backwards
        LinceGetTile(data->walking_tileset, (vec2){0,3}, (vec2){24,24}, (vec2){1,1}),
        LinceGetTile(data->walking_tileset, (vec2){2,3}, (vec2){24,24}, (vec2){1,1}),
        // Walking Left
        LinceGetTile(data->walking_tileset, (vec2){0,2}, (vec2){24,24}, (vec2){1,1}),
        LinceGetTile(data->walking_tileset, (vec2){2,2}, (vec2){24,24}, (vec2){1,1}),
        // Walking Right
        LinceGetTile(data->walking_tileset, (vec2){0,1}, (vec2){24,24}, (vec2){1,1}),
        LinceGetTile(data->walking_tileset, (vec2){2,1}, (vec2){24,24}, (vec2){1,1}),
        // Idle Forward
        LinceGetTile(data->walking_tileset, (vec2){1,0}, (vec2){24,24}, (vec2){1,1}),
        // Idle backwards
        LinceGetTile(data->walking_tileset, (vec2){1,3}, (vec2){24,24}, (vec2){1,1}),
        // Idle Left
        LinceGetTile(data->walking_tileset, (vec2){1,2}, (vec2){24,24}, (vec2){1,1}),
        // Idle Right
        LinceGetTile(data->walking_tileset, (vec2){1,1}, (vec2){24,24}, (vec2){1,1}),
    };

    data->player_anims[ANIM_FRONT     ] = LinceCreateTileAnim(&(LinceTileAnim){.frames=player_tiles   , .frame_count=2, .frame_time=300.0f});
    data->player_anims[ANIM_BACK      ] = LinceCreateTileAnim(&(LinceTileAnim){.frames=player_tiles+2 , .frame_count=2, .frame_time=300.0f});
    data->player_anims[ANIM_LEFT      ] = LinceCreateTileAnim(&(LinceTileAnim){.frames=player_tiles+4 , .frame_count=2, .frame_time=300.0f});
    data->player_anims[ANIM_RIGHT     ] = LinceCreateTileAnim(&(LinceTileAnim){.frames=player_tiles+6 , .frame_count=2, .frame_time=300.0f});
    data->player_anims[ANIM_FRONT_IDLE] = LinceCreateTileAnim(&(LinceTileAnim){.frames=player_tiles+8 , .frame_count=1, .frame_time=300.0f});
    data->player_anims[ANIM_BACK_IDLE ] = LinceCreateTileAnim(&(LinceTileAnim){.frames=player_tiles+9 , .frame_count=1, .frame_time=300.0f});
    data->player_anims[ANIM_LEFT_IDLE ] = LinceCreateTileAnim(&(LinceTileAnim){.frames=player_tiles+10, .frame_count=1, .frame_time=300.0f});
    data->player_anims[ANIM_RIGHT_IDLE] = LinceCreateTileAnim(&(LinceTileAnim){.frames=player_tiles+11, .frame_count=1, .frame_time=300.0f});

    data->current_anim = ANIM_BACK_IDLE;

    // chicken idle animation
    LinceTile chicken_tiles[] = {
        LinceGetTile(data->tileset, (vec2){0,1}, (vec2){16,16}, (vec2){1, 1}),
        LinceGetTile(data->tileset, (vec2){0,0}, (vec2){16,16}, (vec2){1, 1})
    };
    data->chicken_anim = LinceCreateTileAnim(&(LinceTileAnim){
        .frames = chicken_tiles,
        .frame_count = 2,
        .frame_time = 1000.0f,
        .on_repeat = ChickenLoops,
        .on_finish = ChickenEnds,
        .repeats = 4
    });

}

void TestLayerOnDetach(LinceLayer* layer) {
    TestLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' detached", data->name);

    LinceDeleteTexture(data->tex_front);
    LinceDeleteTexture(data->tex_back);
    LinceDeleteTexture(data->walking_tileset);
    LinceDeleteCamera(data->cam);

    for(int i = 0; i != ANIM_COUNT; ++i){
        LinceDeleteTileAnim(data->player_anims[i]);
        data->player_anims[i] = NULL;
    }

    LinceDeleteTileAnim(data->chicken_anim);

    free(data);
}


void TestLayerOnUpdate(LinceLayer* layer, float dt) {
    TestLayer* data = LinceGetLayerData(layer);
    data->dt = dt;
    LinceUILayer* ui = LinceGetAppState()->ui;

    LinceResizeCameraView(data->cam, LinceGetAspectRatio());
	LinceUpdateCamera(data->cam);
    
    // User Input
    const float cam_speed = data->cam_speed;
    const float zoom      = data->cam->zoom;
    const float dr = cam_speed * dt * zoom;

    // camera & player movement
    uint8_t next_anim = data->current_anim;

    if (LinceIsKeyPressed(LinceKey_w)){
        data->cam->pos[1] += dr;
        next_anim = ANIM_FRONT;
    }
    if (LinceIsKeyPressed(LinceKey_s)){
        data->cam->pos[1] -= dr;
        next_anim = ANIM_BACK;
    }
    if (LinceIsKeyPressed(LinceKey_d)){
        data->cam->pos[0] += dr;
        next_anim = ANIM_RIGHT;
    }
    if (LinceIsKeyPressed(LinceKey_a)){
        data->cam->pos[0] -= dr;
        next_anim = ANIM_LEFT;
    }
    if(
        !LinceIsKeyPressed(LinceKey_w) && 
        !LinceIsKeyPressed(LinceKey_s) &&
        !LinceIsKeyPressed(LinceKey_d) &&
        !LinceIsKeyPressed(LinceKey_a)
    ){
        switch(data->current_anim){
        case ANIM_FRONT: next_anim = ANIM_FRONT_IDLE; break;
        case ANIM_BACK:  next_anim = ANIM_BACK_IDLE;  break;
        case ANIM_RIGHT: next_anim = ANIM_RIGHT_IDLE; break;
        case ANIM_LEFT:  next_anim = ANIM_LEFT_IDLE;  break;
        default: break;
        };
    }

    if(next_anim != data->current_anim){
        LinceResetTileAnim(data->player_anims[data->current_anim]);
    }
    data->current_anim = next_anim;

    LinceUIText(ui, "DebugFPS", 20, 20, LinceFont_Droid30, 10, "FPS: %.0f", 1000.0/dt);
    LinceUIText(ui, "DebugDT",  20, 42, LinceFont_Droid30, 15, "dt: %.1f ms", dt);

    LinceBeginScene(data->cam);

    // Tilemap
    for(uint32_t i = 0; i != tm_width; ++i){
        for(uint32_t j = 0; j != tm_height; ++j){
            uint32_t index = j * tm_width + i;
            int tile = tilemap[index];
            LinceDrawQuad((LinceQuadProps){
                .x = (float)i - (float)tm_width / 2.0f,
                .y = (float)j - (float)tm_height / 2.0f,
                .w=1.001f, .h=1.001f, // must slightly overlap to avoid black lines
                .color={1,1,1,1},
                .tile = &data->tiles[tile],
                .zorder = 0.1
            });
        }
    }

    // Trees
    LinceDrawQuad((LinceQuadProps){
        .x=4.0f, .y=4.0f,
        .w=2.0f, .h=2.0f,
        .color={1,1,1,1},
        .tile = &data->tiles[TILE_TREE],
        .zorder = 0.5
    });
    LinceDrawQuad((LinceQuadProps){
        .x=-2.0f, .y=-2.0f,
        .w=2.0f, .h=2.0f,
        .color={1,1,1,1},
        .tile = &data->tiles[TILE_TREE],
        .zorder = 0.5
    });

    // Chicken
    LinceUpdateTileAnim(data->chicken_anim, dt);
    LinceDrawQuad((LinceQuadProps){
        .x=5.0f, .y=3.0f,
        .w=1.0f, .h=1.0f,
        .color={1,1,1,1},
        .tile = data->chicken_anim->current_tile,
        .zorder = 0.5
    });

    // PLAYER
    LinceTileAnim* anim = data->player_anims[data->current_anim];
    LinceUpdateTileAnim(anim, dt);
    LinceDrawQuad((LinceQuadProps){
        .x=data->cam->pos[0],
        .y=data->cam->pos[1],
        .w=1.5f, .h=1.5f,
        .color={1,1,1,1},
        .tile = anim->current_tile,
        .zorder = 0.6
    });
    
    LinceEndScene();
}

void TestLayerOnEvent(LinceLayer* layer, LinceEvent* event){
    if(event->type == LinceEventType_MouseScrolled){
        LinceMouseScrolledEvent* scroll = event->data.MouseScrolled;
        TestLayer* data = LinceGetLayerData(layer);
        data->cam->zoom *= powf(0.80, scroll->yoff); // * 0.5 * dt;
    }
}

LinceLayer* TestLayerInit(char* name) {

    TestLayer* my_layer = calloc(1, sizeof(TestLayer));
    LINCE_ASSERT(my_layer, "Failed to allocate layer data");

    size_t len_orig = strlen(name);
    size_t len = len_orig < LINCE_NAME_MAX ? len_orig : LINCE_NAME_MAX;
    memcpy(my_layer->name, name, len);

    LinceLayer* layer = LinceCreateLayer(my_layer);
    layer->OnAttach = TestLayerOnAttach;
    layer->OnDetach = TestLayerOnDetach;
    layer->OnEvent  = TestLayerOnEvent;
    layer->OnUpdate = TestLayerOnUpdate;

    return layer;
}


// =============================================================


void GameInit() {
	LINCE_INFO("\n User App Initialised");
    LincePushLayer(TestLayerInit("Test"));
    //LincePushLayer(NKLayerInit());
    //LincePushLayer(PongLayerInit());
    //LincePushLayer(MCommandLayerInit());
}

void GameOnUpdate(float dt) {
    LinceCheckErrors();
    LINCE_UNUSED(dt);
}

void GameTerminate() {
    LINCE_INFO(" User App Terminated");
}

int main(int argc, const char* argv[]) {

    #ifdef LINCE_DEBUG
    LINCE_INFO(" --- DEBUG MODE --- ");
    #endif

    LinceApp* app = LinceGetAppState();

    // app->user_data = NULL;
    app->screen_width = 900;
    app->screen_height = 600;
    app->title = "The Legend of Cheesus Christ";
    // app->options = LINCE_FULLSCREEN | LINCE_VSYNC | LINCE_RESIZEABLE | ...

    app->game_init = GameInit;
    app->game_on_update = GameOnUpdate;
    // app->game_on_event = GameOnEvent;
    app->game_terminate = GameTerminate;
    
    LinceRun();

    LINCE_UNUSED(argc);
    LINCE_UNUSED(argv);
    return 0;
}
