
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

#include "renderer/tileset.h"

enum TileNames {
    TILE_GRASS = 0,
    TILE_DIRT,
    TILE_TREE,
    TILE_CHICKEN,
    TILE_COUNT
};

enum WalkingTiles {
    TILE_FRONT,
    TILE_BACK,
    TILE_LEFT,
    TILE_RIGHT,
    TILE_WALKING_COUNT
};

typedef struct MyLayer {
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
    LinceTile walking_tiles[TILE_WALKING_COUNT];
    uint8_t player_facing;

    vec4 color;
    LinceCamera* cam;
} MyLayer;

void MyLayerOnAttach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' attached", data->name);

    data->cam = LinceCreateCamera(LinceGetAspectRatio());

    data->red = 0.0f;
    data->vel = 5e-4f;
    data->cam_speed = 3e-3f;
    data->cam->zoom = 3.0;
    data->color_step = 0.003f;
    
    data->tex_front = LinceCreateTexture("PatrickF", "lince/assets/front.png");
    data->tex_back  = LinceCreateTexture("PatrickB", "lince/assets/back.png");
    data->tileset = LinceCreateTexture("Tileset", "game/assets/textures/shubibubi-cozy-farm.png");
    data->walking_tileset = LinceCreateTexture("Walking", "game/assets/textures/elv-games-movement.png");

    data->tiles[TILE_GRASS]   = LinceGetTile(data->tileset, (vec2){1,8}, (vec2){16,16}, (vec2){1, 1});
    data->tiles[TILE_DIRT]    = LinceGetTile(data->tileset, (vec2){5,9}, (vec2){16,16}, (vec2){1, 1});
    data->tiles[TILE_TREE]    = LinceGetTile(data->tileset, (vec2){9,5}, (vec2){16,16}, (vec2){2, 2});
    data->tiles[TILE_CHICKEN] = LinceGetTile(data->tileset, (vec2){0,1}, (vec2){16,16}, (vec2){1, 1});

    data->walking_tiles[TILE_FRONT] = LinceGetTile(data->walking_tileset, (vec2){1,0}, (vec2){24,24}, (vec2){1,1});
    data->walking_tiles[TILE_BACK]  = LinceGetTile(data->walking_tileset, (vec2){1,3}, (vec2){24,24}, (vec2){1,1});
    data->walking_tiles[TILE_LEFT]  = LinceGetTile(data->walking_tileset, (vec2){1,2}, (vec2){24,24}, (vec2){1,1});
    data->walking_tiles[TILE_RIGHT] = LinceGetTile(data->walking_tileset, (vec2){1,1}, (vec2){24,24}, (vec2){1,1});

    data->player_facing = TILE_FRONT;
    
}

void MyLayerOnDetach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' detached", data->name);

    LinceDeleteTexture(data->tex_front);
    LinceDeleteTexture(data->tex_back);
    LinceDeleteTexture(data->walking_tileset);
    LinceDeleteCamera(data->cam);

    free(data);
}


void MyLayerOnUpdate(LinceLayer* layer, float dt) {
    MyLayer* data = LinceGetLayerData(layer);
    data->dt = dt;
    LinceUILayer* ui = LinceGetAppState()->ui;

    LinceResizeCameraView(data->cam, LinceGetAspectRatio());
	LinceUpdateCamera(data->cam);
    
    // User Input
    const float cam_speed = data->cam_speed;
    const float zoom      = data->cam->zoom;
    const float dr = cam_speed * dt * zoom;

    // camera & player movement
    if (LinceIsKeyPressed(LinceKey_w)){
        data->cam->pos[1] += dr;
        data->player_facing = TILE_FRONT;
    }
    if (LinceIsKeyPressed(LinceKey_s)){
        data->cam->pos[1] -= dr;
        data->player_facing = TILE_BACK;
    }
    if (LinceIsKeyPressed(LinceKey_d)){
        data->cam->pos[0] += dr;
        data->player_facing = TILE_RIGHT;
    }
    if (LinceIsKeyPressed(LinceKey_a)){
        data->cam->pos[0] -= dr;
        data->player_facing = TILE_LEFT;
    }

    LinceUIText(ui, "DebugFPS", 20, 20, LinceFont_Droid30, 10, "FPS: %.0f", 1000.0/dt);
    LinceUIText(ui, "DebugDT",  20, 42, LinceFont_Droid30, 15, "dt: %.1f ms", dt);

    LinceBeginScene(data->cam);

    int gridx = 10;
    int gridy = 10;

    float r, g, b;
    for(int i = 0; i != gridx; ++i){
        for(int j = 0; j != gridy; ++j){
            r = (float)i/(float)gridx;
            b = (float)j/(float)gridy;
            g = 0.8 - b - r;
            LinceDrawQuad( (LinceQuadProps){
                .x = (float)i - (float)gridx / 2.0,
                .y = (float)j - (float)gridy / 2.0,
                .w = 0.95, .h = 0.95,
                .color = {r, g, b, 1.0}
            });
        }
    }

    LinceDrawQuad( (LinceQuadProps){
        .x=-0.5, .y=0.0, .w=0.9, .h=0.9,
        .color={1,1,1,1},
        .rotation = 45,
        .texture=data->tex_front,
        .zorder = 0.1
    });
    LinceDrawQuad( (LinceQuadProps){
        .x=0.0, .y=0.0, .w=0.9, .h=0.9,
        .color={1,1,1,1},
        .rotation = 45,
        .texture=data->tex_back,
        .zorder = 0.9
    });
    LinceDrawQuad( (LinceQuadProps){
        .x=0.5, .y=0.0, .w=0.9, .h=0.9,
        .color={1,1,1,1},
        .rotation = 45,
        .texture=data->tex_front,
        .zorder = 0.1
    });

    for(int i = 0; i != 10; ++i){
        for(int j = 0; j != 10; ++j){
            LinceDrawQuad((LinceQuadProps){
                .x=(float)i, .y=(float)j,
                .w=1.0f, .h=1.0f,
                .color={1,1,1,1},
                .tile = &data->tiles[TILE_GRASS],
                .zorder = 0.1
            });
        }
    }

    LinceDrawQuad((LinceQuadProps){
        .x=5.0f, .y=5.0f,
        .w=2.0f, .h=2.0f,
        .color={1,1,1,1},
        .tile = &data->tiles[TILE_TREE],
        .zorder = 0.5
    });

    LinceDrawQuad((LinceQuadProps){
        .x=5.0f, .y=3.0f,
        .w=1.0f, .h=1.0f,
        .color={1,1,1,1},
        .tile = &data->tiles[TILE_CHICKEN],
        .zorder = 0.5
    });

    // PLAYER
    LinceDrawQuad((LinceQuadProps){
        .x=data->cam->pos[0],
        .y=data->cam->pos[1],
        .w=1.5f, .h=1.5f,
        .color={1,1,1,1},
        .tile = &data->walking_tiles[data->player_facing],
        .zorder = 0.6
    });
    
    LinceEndScene();
}

void MyLayerOnEvent(LinceLayer* layer, LinceEvent* event){
    if(event->type == LinceEventType_MouseScrolled){
        LinceMouseScrolledEvent* scroll = event->data.MouseScrolled;
        MyLayer* data = LinceGetLayerData(layer);
        data->cam->zoom *= powf(0.80, scroll->yoff); // * 0.5 * dt;
    }
}


LinceLayer* MyLayerInit(char* name) {

    MyLayer* my_layer = calloc(1, sizeof(MyLayer));
    LINCE_ASSERT(my_layer, "Failed to allocate layer data");

    size_t len_orig = strlen(name);
    size_t len = len_orig < LINCE_NAME_MAX ? len_orig : LINCE_NAME_MAX;
    memcpy(my_layer->name, name, len);

    LinceLayer* layer = LinceCreateLayer(my_layer);
    layer->OnAttach = MyLayerOnAttach;
    layer->OnDetach = MyLayerOnDetach;
    layer->OnEvent  = MyLayerOnEvent;
    layer->OnUpdate = MyLayerOnUpdate;

    return layer;
}


// =============================================================


void GameInit() {
	LINCE_INFO("\n User App Initialised");
    LincePushLayer(MyLayerInit("Test"));
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
