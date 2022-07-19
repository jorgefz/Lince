
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


typedef struct SpriteAnim {
	LinceTile* frames;			// array of animation frames
	uint32_t frame_count;	// number of animation frames

	float frametime;		// duration of each frame in ms
	float time;				// time counter between frames
	uint32_t current_frame;	// current active frame to render

	bool loop;				// indicates wether animation loops forever, unused
} SpriteAnim;


SpriteAnim* CreateAnim(LinceTile* frames, uint32_t frame_count, float frametime, uint32_t start_frame, bool loop){

	SpriteAnim* anim = calloc(1, sizeof(SpriteAnim));
	assert(anim);

	anim->frames = malloc(sizeof(LinceTile) * frame_count);
	assert(anim->frames);
	memmove(anim->frames, frames, sizeof(LinceTile) * frame_count);

	anim->frame_count = frame_count;
	anim->frametime = frametime;
	anim->time = frametime;
	anim->current_frame = start_frame;
	anim->loop = loop;

	return anim;
}

void UpdateAnim(SpriteAnim* anim, float dt){
	anim->time -= dt;

	if(anim->time <= 0.0f){
		anim->current_frame++;
		if(anim->current_frame >= anim->frame_count){
			anim->current_frame = 0;
		}
		anim->time = anim->frametime;
	}

}

void ResetAnim(SpriteAnim* anim){
	anim->time = anim->frametime;
	anim->current_frame = 0;
}


void DeleteAnim(SpriteAnim* anim){
	if(!anim) return;
	if(anim->frames) free(anim->frames);
	free(anim);
}

enum TileNames {
    TILE_GRASS = 0,
    TILE_DIRT,
    TILE_TREE,
    TILE_CHICKEN,
    TILE_COUNT
};

enum WalkingAnims {
    ANIM_FRONT,
    ANIM_FRONT_IDLE,
    ANIM_BACK,
    ANIM_BACK_IDLE,
    ANIM_LEFT,
    ANIM_LEFT_IDLE,
    ANIM_RIGHT,
    ANIM_RIGHT_IDLE,
    ANIM_COUNT
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
    SpriteAnim* player_anims[ANIM_COUNT];
    uint8_t current_anim;

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

    LinceTile front_walk_tiles[] = {
        LinceGetTile(data->walking_tileset, (vec2){0,0}, (vec2){24,24}, (vec2){1,1}),
        LinceGetTile(data->walking_tileset, (vec2){2,0}, (vec2){24,24}, (vec2){1,1}),
    };
    LinceTile front_idle_tiles[] = {
        LinceGetTile(data->walking_tileset, (vec2){1,0}, (vec2){24,24}, (vec2){1,1}),
    };
    LinceTile back_walk_tiles[] = {
        LinceGetTile(data->walking_tileset, (vec2){0,3}, (vec2){24,24}, (vec2){1,1}),
        LinceGetTile(data->walking_tileset, (vec2){2,3}, (vec2){24,24}, (vec2){1,1}),
    };
    LinceTile back_idle_tiles[] = {
        LinceGetTile(data->walking_tileset, (vec2){1,3}, (vec2){24,24}, (vec2){1,1}),
    };
    LinceTile right_walk_tiles[] = {
        LinceGetTile(data->walking_tileset, (vec2){0,1}, (vec2){24,24}, (vec2){1,1}),
        LinceGetTile(data->walking_tileset, (vec2){2,1}, (vec2){24,24}, (vec2){1,1}),
    };
    LinceTile right_idle_tiles[] = {
        LinceGetTile(data->walking_tileset, (vec2){1,1}, (vec2){24,24}, (vec2){1,1}),
    };
    LinceTile left_walk_tiles[] = {
        LinceGetTile(data->walking_tileset, (vec2){0,2}, (vec2){24,24}, (vec2){1,1}),
        LinceGetTile(data->walking_tileset, (vec2){2,2}, (vec2){24,24}, (vec2){1,1}),
    };
    LinceTile left_idle_tiles[] = {
        LinceGetTile(data->walking_tileset, (vec2){1,2}, (vec2){24,24}, (vec2){1,1}),
    };

    data->player_anims[ANIM_FRONT]      = CreateAnim(front_walk_tiles, 2, 300.0f, 0, 0);
    data->player_anims[ANIM_BACK]       = CreateAnim(back_walk_tiles, 2, 300.0f, 0, 0);
    data->player_anims[ANIM_LEFT]       = CreateAnim(left_walk_tiles, 2, 300.0f, 0, 0);
    data->player_anims[ANIM_RIGHT]      = CreateAnim(right_walk_tiles, 2, 300.0f, 0, 0);
    data->player_anims[ANIM_FRONT_IDLE] = CreateAnim(front_idle_tiles, 1, 300.0f, 0, 0);
    data->player_anims[ANIM_BACK_IDLE]  = CreateAnim(back_idle_tiles, 1, 300.0f, 0, 0);
    data->player_anims[ANIM_LEFT_IDLE]  = CreateAnim(left_idle_tiles, 1, 300.0f, 0, 0);
    data->player_anims[ANIM_RIGHT_IDLE] = CreateAnim(right_idle_tiles, 1, 300.0f, 0, 0);

    data->current_anim = ANIM_BACK_IDLE;
    
}

void MyLayerOnDetach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' detached", data->name);

    LinceDeleteTexture(data->tex_front);
    LinceDeleteTexture(data->tex_back);
    LinceDeleteTexture(data->walking_tileset);
    LinceDeleteCamera(data->cam);

    for(int i = 0; i != ANIM_COUNT; ++i){
        DeleteAnim(data->player_anims[i]);
        data->player_anims[i] = NULL;
    }

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
        ResetAnim(data->player_anims[data->current_anim]);
    }
    data->current_anim = next_anim;

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
    /*
    LinceDrawQuad((LinceQuadProps){
        .x=data->cam->pos[0],
        .y=data->cam->pos[1],
        .w=1.5f, .h=1.5f,
        .color={1,1,1,1},
        .tile = &data->walking_tiles[data->player_facing],
        .zorder = 0.6
    });
    */
    SpriteAnim* anim = data->player_anims[data->current_anim];
    UpdateAnim(anim, dt);
    LinceDrawQuad((LinceQuadProps){
        .x=data->cam->pos[0],
        .y=data->cam->pos[1],
        .w=1.5f, .h=1.5f,
        .color={1,1,1,1},
        .tile = &anim->frames[anim->current_frame],
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
