
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

//#include "renderer/tileset.h"
//#include "renderer/tile_anim.h"


float NormZ(float y, vec2 ylim, vec2 zlim){
    float z = (zlim[1] - zlim[0]) * (y - ylim[0]) / (ylim[1]-ylim[0]) + zlim[0];
    return 1.0f - z;
}


LinceTile* LoadTilesFromTexture(const char* texture_file, size_t* length, float px){
    LinceTexture* tex = LinceCreateTexture("tileset", texture_file);
    size_t xtiles = tex->width / (uint32_t)px;
    size_t ytiles = tex->height / (uint32_t)px;
    LinceTile tile;

    LinceTile* tiles = malloc(sizeof(LinceTile)*xtiles*ytiles);
    LINCE_ASSERT_ALLOC(tiles, sizeof(LinceTile)*xtiles*ytiles);

    for(size_t y = 0; y != ytiles; ++y){
        for(size_t x = 0; x != xtiles; ++x){
            tile = LinceGetTile(tex, (vec2){x,y}, (vec2){px,px}, (vec2){1,1});
            memmove(tiles + y*xtiles + x, &tile, sizeof(LinceTile));
        }
    }

    *length = xtiles * ytiles;
    return tiles;
}




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
uint32_t tilemap_data[] = {
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


typedef struct LinceTilemap{
    LinceTile* tileset;
    size_t tileset_count;

    size_t width, height;
    vec2 offset;
    uint32_t* tiledata;
    uint8_t* collision_data;

    LinceTile* overlay_tiles;
    vec2* overlay_positions;
    size_t overlay_count;

    /*
    CallbackFn on_teleport(this, target-tilemap, trigger, ...)

    */
    
} LinceTilemap;



LinceTilemap* LinceCreateTilemap(LinceTilemap* props){

    LINCE_ASSERT(props, "Tilemap missing");
    LINCE_ASSERT(props->tileset, "Tileset missing");
    LINCE_ASSERT(props->tileset_count > 0, "Tileset must have more than zero tiles");
    LINCE_ASSERT(props->tiledata, "Tile data missing");
    LINCE_ASSERT(props->width > 0 && props->height > 0,
        "Tilemap size must be greater than zero");

    size_t tm_size = props->width * props->height;

    // Ensure no index in tile data is greater than the tileset size
    for(size_t i = 0; i != tm_size; ++i){
        LINCE_ASSERT(props->tiledata[i] < props->tileset_count,
            "Invalid value in tilemap data "
            "(tile index %d but there are only %d tiles)",
            (int)props->tiledata[i], (int)props->tileset_count);
    }

    // Allocate main data and copy settings
    LinceTilemap* tm = malloc(sizeof(LinceTilemap));
    LINCE_ASSERT_ALLOC(tm, sizeof(LinceTilemap));
    memmove(tm, props, sizeof(LinceTilemap));

    // Copy tile data
    tm->tiledata = malloc(sizeof(uint32_t) * tm_size);
    LINCE_ASSERT_ALLOC(tm->tiledata, sizeof(uint32_t) * tm_size);
    memmove(tm->tiledata, props->tiledata, sizeof(uint32_t) * tm_size);

    // NOTE: tileset is not copied!!

    return tm;
}


void LinceDrawTilemap(LinceTilemap* tm){
    if(!tm) return;

    // Draw ground tiles
    for(size_t j = 0; j != tm->height; ++j){
        for(size_t i = 0; i != tm->width; ++i){
            LinceDrawQuad((LinceQuadProps){
                .x = (float)i - tm->offset[0],
                .y = (float)j - tm->offset[1],
                .w = 1.001f, .h = 1.001f,
                .color = {1,1,1,1},
                .tile = &tm->tileset[tm->tiledata[j*tm->width + i]],
                .zorder = 0.0
            });
        }
    }

    if(!tm->overlay_tiles|| !tm->overlay_positions) return;

    // Draw overlay tiles
    for(size_t i = 0; i != tm->overlay_count; ++i){
        LinceTile* tile = &tm->overlay_tiles[i];
        LinceDrawQuad((LinceQuadProps){
            .x = tm->overlay_positions[i][0] - tm->offset[0],
            .y = tm->overlay_positions[i][1] - tm->offset[1],
            .w = tile->tilesize[0],
            .h = tile->tilesize[1],
            .color = {1,1,1,1},
            .tile = tile,
            .zorder = NormZ(
                tm->overlay_positions[i][1] - tm->offset[1] - tile->tilesize[1]/2.0f,
                (vec2){-100.0f, 100.0f},
                (vec2){0.1f, 1.0f}
            )
        });
    }

}

void LinceDeleteTilemap(LinceTilemap* tm){
    if(!tm) return;
    if(tm->tiledata) free(tm->tiledata);
    free(tm);
}

int GetTilemapIndexAtMouse(LinceTilemap* tm, LinceCamera* cam, vec2 xy_ind){
    vec2 mouse_pos;
    LinceGetMousePosWorld(mouse_pos, cam);

    int tm_x = floorf(mouse_pos[0] + tm->offset[0] + 0.5f);
    int tm_y = floorf(mouse_pos[1] + tm->offset[1] + 0.5f);

    xy_ind[0] = (float)tm_x;
    xy_ind[1] = (float)tm_y;

    if(tm_x >= 0 && tm_x < (int)tm->width &&
       tm_y >= 0 && tm_y < (int)tm->height) {
        return (int)(tm_y * tm->width + tm_x);
    }
    return -1;
}

/*
Returns the tile of a tilemap over which the mouse pointer is hovering.
If no tile is present, it returns NULL.
*/
LinceTile* GetTileAtMouse(LinceTilemap* tm, LinceCamera* cam){

    vec2 mouse_pos;
    LinceGetMousePosWorld(mouse_pos, cam);

    int tm_x = (int)(mouse_pos[0] + tm->offset[0]);
    int tm_y = (int)(mouse_pos[1] + tm->offset[1]);

    if(tm_x >= 0 && tm_x < (int)tm->width &&
       tm_y >= 0 && tm_y < (int)tm->height) {
        size_t idx = tm->tiledata[tm_y * tm->width + tm_x];
        return &tm->tileset[idx];
    }
    return NULL;
}




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
    LINCE_UNUSED(anim);
    LINCE_UNUSED(args);
    printf("Chicken repeats\n");
}

void ChickenEnds(LinceTileAnim* anim, void* args){
    LINCE_UNUSED(anim);
    LINCE_UNUSED(args);
    printf("Chicken stopped moving!\n");
}



// TEST LAYER

typedef struct TestLayer {
    char name[LINCE_NAME_MAX];
    float dt;
    float cam_speed;

    LinceCamera* cam;
    LinceVertexArray* va;
    LinceVertexBuffer vb;
    LinceIndexBuffer ib;
    LinceShader* shader;
    LinceTexture* tex_front;
    LinceTexture* tex_back;
    
    LinceTexture* walking_tileset;

    LinceTile tiles[TILE_COUNT];
    uint8_t current_anim;

    uint32_t player_anim_order[ANIM_COUNT*2];
    LinceTileAnim* player_anim;
    LinceTileAnim* chicken_anim;

    LinceTexture* tileset;
    LinceTilemap* tilemap;

    int chosen_menu_tile;

} TestLayer;


void DrawHoverTile(TestLayer* data){
    
    LinceTile* hover_tile = GetTileAtMouse(data->tilemap, data->cam);
    LinceDrawQuad((LinceQuadProps){
        .x = data->cam->pos[0] - 0.5f,
        .y = data->cam->pos[1],
        .w = 0.35f, .h = 0.35f,
        .color = {0,0,0,1},
        .zorder = 0.85
    });
    if(hover_tile) {
        LinceDrawQuad((LinceQuadProps){
            .x = data->cam->pos[0]- 0.5f,
            .y = data->cam->pos[1],
            .w = 0.3f, .h = 0.3f,
            .color = {1,1,1,1},
            .tile = hover_tile,
            .zorder = 0.9
        });
    } else {
        LinceDrawQuad((LinceQuadProps){
            .x = data->cam->pos[0] - 0.5f,
            .y = data->cam->pos[1],
            .w = 0.3f, .h = 0.3f,
            .color = {1,0,0,1},
            .zorder = 0.9
        });
    }

    // Highlight hovered tile
    vec2 xy_ind;
    int chosen_tile = GetTilemapIndexAtMouse(data->tilemap, data->cam, xy_ind);
    if(chosen_tile == -1) return;

    LinceDrawQuad((LinceQuadProps){
        .x = xy_ind[0] - data->tilemap->offset[0],
        .y = xy_ind[1] - data->tilemap->offset[1],
        .w = 1.001f, .h = 1.001f,
        .color = {1,0,0,0.2},
        .zorder = 0.9
    });

}


void TestLayerOnAttach(LinceLayer* layer) {
    TestLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' attached", data->name);
    
    data->cam = LinceCreateCamera(LinceGetAspectRatio());
    data->cam_speed = 9e-4f;
    data->cam->zoom = 4.0;

    data->tex_front = LinceCreateTexture("PatrickF", "game/assets/textures/front.png");
    data->tex_back  = LinceCreateTexture("PatrickB", "game/assets/textures/back.png");
    data->tileset = LinceCreateTexture("Tileset", "game/assets/textures/shubibubi-cozy-farm.png");
    data->walking_tileset = LinceCreateTexture("Walking", "game/assets/textures/elv-games-movement.png");

    // TILEMAP
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

    data->tilemap = LinceCreateTilemap(&(LinceTilemap){
        .tiledata = tilemap_data,
        .width = tm_width,
        .height = tm_height,
        .tileset = data->tiles,
        .tileset_count = TILE_COUNT,
        .offset = {5.0f, 5.0f}
    });

    data->tilemap->overlay_count = 2;
    
    data->tilemap->overlay_tiles = malloc(sizeof(LinceTile)*2);
    memmove(
        data->tilemap->overlay_tiles,
        (LinceTile[]){data->tiles[TILE_TREE],data->tiles[TILE_TREE]},
        sizeof(LinceTile)*2
    );
    
    data->tilemap->overlay_positions = calloc(2, sizeof(vec2));
    memmove(data->tilemap->overlay_positions,
        (vec2[]){{8,7},{3,3}}, sizeof(vec2)*2
    );
    
    // PLAYER MOVEMENT
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
    memmove(data->player_anim_order, order_indices, sizeof(uint32_t)*ANIM_COUNT*2);
    data->current_anim = ANIM_FRONT_IDLE;
    data->player_anim = LinceCreateTileAnim(&(LinceTileAnim){
        .frames = player_tiles,
        .frame_count = sizeof(player_tiles)/sizeof(LinceTile),
        .frame_time = 300.0f,
        .order = order_indices + ANIM_FRONT_IDLE*2,
        .order_count = 2
    });

    // CHICKEN ANIMATION
    LinceTile chicken_tiles[] = {
        LinceGetTile(data->tileset, (vec2){0,1}, (vec2){16,16}, (vec2){1, 1}),
        LinceGetTile(data->tileset, (vec2){1,1}, (vec2){16,16}, (vec2){1, 1}),
        LinceGetTile(data->tileset, (vec2){2,1}, (vec2){16,16}, (vec2){1, 1}),
        LinceGetTile(data->tileset, (vec2){3,1}, (vec2){16,16}, (vec2){1, 1}),
        LinceGetTile(data->tileset, (vec2){0,0}, (vec2){16,16}, (vec2){1, 1}),
        LinceGetTile(data->tileset, (vec2){1,0}, (vec2){16,16}, (vec2){1, 1}),
        LinceGetTile(data->tileset, (vec2){2,0}, (vec2){16,16}, (vec2){1, 1}),
        LinceGetTile(data->tileset, (vec2){3,0}, (vec2){16,16}, (vec2){1, 1}),
    };
    data->chicken_anim = LinceCreateTileAnim(&(LinceTileAnim){
        .frames = chicken_tiles,
        .frame_count = 8,
        .frame_time = 400.0f,
        .on_repeat = ChickenLoops,
        .on_finish = ChickenEnds,
        .repeats = 0,
        .start = 1,
        .order = (uint32_t[]){0,1,4,5},
        .order_count = 4
    });

    data->chosen_menu_tile = -1;
}

void TestLayerOnDetach(LinceLayer* layer) {
    TestLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' detached", data->name);

    LinceDeleteTexture(data->tex_front);
    LinceDeleteTexture(data->tex_back);
    LinceDeleteTexture(data->walking_tileset);
    LinceDeleteCamera(data->cam);

    LinceDeleteTileAnim(data->player_anim);
    LinceDeleteTileAnim(data->chicken_anim);

    LinceDeleteTilemap(data->tilemap);

    free(data);
}

void DrawGUI(TestLayer* data){
    LinceUILayer* ui = LinceGetAppState()->ui;
    
    nk_style_set_font(ui->ctx, &ui->fonts[LinceFont_Droid15]->handle);
    
    if (nk_begin(ui->ctx, "Demo", nk_rect(10, 10, 300, 600),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE
    )) {
        
        /*
        size_t cols, rows;
        for(size_t i = 0; i != rows; ++i){
            nk_layout_row_dynamic(ui->ctx, 200, cols);
            for(size_t j = 0; j != cols; ++j){
                // get tile subimage
                if(nk_button_image(ui->ctx, subimage)){
                    // tile pressed callback
                }
            }

        }
        */


       for(size_t i = 0; i != 4; ++i){
            nk_layout_row_dynamic(ui->ctx, 70, 4);
            for(size_t j = 0; j != 4; ++j){
                LinceTile* tile = &data->tiles[i*4 + j];
                struct nk_rect rect = {
                    .x = tile->pos[0]      * tile->cellsize[0],
                    .y = tile->pos[1]      * tile->cellsize[1],
                    .w = tile->tilesize[0] * tile->cellsize[0],
                    .h = tile->tilesize[1] * tile->cellsize[1]        
                };
                struct nk_image img = nk_subimage_id(
                    data->tileset->id,
                    data->tileset->width,
                    data->tileset->height,
                    rect
                );
                if( nk_button_image(ui->ctx, img) ){
                    data->chosen_menu_tile = i*4 + j;
                }
            }
       }

    }
    nk_end(ui->ctx);

}


void TestLayerOnUpdate(LinceLayer* layer, float dt) {
    TestLayer* data = LinceGetLayerData(layer);
    data->dt = dt;
    LinceUILayer* ui = LinceGetAppState()->ui;

    // UI
    DrawGUI(data);

    LinceResizeCameraView(data->cam, LinceGetAspectRatio());
	LinceUpdateCamera(data->cam);
    
    // User Input
    const float cam_speed = data->cam_speed;
    const float zoom      = data->cam->zoom;
    const float dr = cam_speed * dt * zoom;

    // camera & player movement
    uint32_t next_anim = data->current_anim;

    enum {IDLE=0x0, UP=0x1, DOWN=0x2, LEFT=0x4, RIGHT=0x8};
    uint8_t direction = IDLE;
    direction |= LinceIsKeyPressed(LinceKey_w) * UP;
    direction |= LinceIsKeyPressed(LinceKey_s) * DOWN;
    direction |= LinceIsKeyPressed(LinceKey_a) * LEFT;
    direction |= LinceIsKeyPressed(LinceKey_d) * RIGHT;

    if (direction & UP){
        data->cam->pos[1] += dr;
        next_anim = ANIM_FRONT;
    }
    if (direction & DOWN){
        data->cam->pos[1] -= dr;
        next_anim = ANIM_BACK;
    }
    if (direction & RIGHT){
        data->cam->pos[0] += dr;
        next_anim = ANIM_RIGHT;
    }
    if (direction & LEFT){
        data->cam->pos[0] -= dr;
        next_anim = ANIM_LEFT;
    }
    if(direction == IDLE){
        switch(data->current_anim){
        case ANIM_FRONT: next_anim = ANIM_FRONT_IDLE; break;
        case ANIM_BACK:  next_anim = ANIM_BACK_IDLE;  break;
        case ANIM_RIGHT: next_anim = ANIM_RIGHT_IDLE; break;
        case ANIM_LEFT:  next_anim = ANIM_LEFT_IDLE;  break;
        default: break;
        };
    }

    data->player_anim->order[0] = data->player_anim_order[next_anim*2];
    data->player_anim->order[1] = data->player_anim_order[next_anim*2+1];

    if(next_anim != data->current_anim){        
        LinceResetTileAnim(data->player_anim);
        data->current_anim = next_anim;
    }

    LinceUIText(ui, "DebugFPS", 20, 20, LinceFont_Droid30, 10, "FPS: %.0f", 1000.0/dt);
    LinceUIText(ui, "DebugDT",  20, 42, LinceFont_Droid30, 15, "dt: %.1f ms", dt);
    
    LinceBeginScene(data->cam);

    // Tilemap
    LinceDrawTilemap(data->tilemap);

    // Chicken
    LinceUpdateTileAnim(data->chicken_anim, dt);
    LinceDrawQuad((LinceQuadProps){
        .x=5.0f, .y=3.0f,
        .w=1.0f, .h=1.0f,
        .color={1,1,1,1},
        .tile = data->chicken_anim->current_tile,
        .zorder = NormZ(3.0f-0.5f, (vec2){-100.0f,100.0f}, (vec2){0.1f,1.0f})
    });

    // PLAYER
    LinceUpdateTileAnim(data->player_anim, dt);
    LinceDrawQuad((LinceQuadProps){
        .x=data->cam->pos[0],
        .y=data->cam->pos[1],
        .w=1.5f, .h=1.5f,
        .color={1,1,1,1},
        .tile =  data->player_anim->current_tile,
        .zorder = NormZ(data->cam->pos[1]-0.75f, (vec2){-100.0f,100.0f}, (vec2){0.1f,1.0f})
    });

    // Selected tile
    DrawHoverTile(data);

    
    LinceEndScene();
}

void TestLayerOnEvent(LinceLayer* layer, LinceEvent* event){
    TestLayer* data = LinceGetLayerData(layer);

    if(event->type == LinceEventType_MouseScrolled){
        LinceMouseScrolledEvent* scroll = event->data.MouseScrolled;    
        data->cam->zoom *= powf(0.80, scroll->yoff); // * 0.5 * dt;
        return;
    }

    if(event->type == LinceEventType_MouseButtonPressed){
        LinceMouseButtonPressedEvent* press = event->data.MouseButtonPressed;
        if(press->button != LinceMouseButton_1) return;
        
        vec2 xy_ind;
        int change_tile = GetTilemapIndexAtMouse(data->tilemap, data->cam, xy_ind);
        if(change_tile == -1 || data->chosen_menu_tile == -1) return;
        data->tilemap->tiledata[change_tile] = data->chosen_menu_tile;
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
    app->title = "Sandbox";
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
