
#include "world_scene.h"
#include "gamedata.h"

/*
Left to right,
top to bottom.
*/
typedef enum OutsideTilenames {
    Tile_Green=0,    Tile_Grass=1,      Tile_Path=2,        Tile_Post=3,        Tile_Fence=4,
    Tile_RockMed=5,  Tile_RockS=6,      Tile_RockL=7,       Tile_FenceUL=8,     Tile_FenceUR=9,
    Tile_PlanksL=10, Tile_Planks=11,    Tile_PlanksR=12,    Tile_FenceLL=13,    Tile_FenceLR=14,
    Tile_Door=15,    Tile_RoofBackL=16, Tile_RoofBackR=17,  Tile_RoofFrontL=18, Tile_RoofFrontR=19,
    Tile_Count
} OutsideTilenames;

static uint32_t OUTSIDE_GRID[] = {
    0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 1,16,17, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 0, 1,16,18,19,17, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0,18,24,24,19, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0,10,11,11,12, 0, 0, 0, 0, 0, 0,
    1, 0, 1, 0, 0,10,11,15,12, 0, 0, 0, 0, 0, 0,
    0, 0, 0,23, 0, 0, 1, 0, 0, 1,23, 1, 0, 0, 0,
    1, 0, 0,13, 4, 4, 0, 4, 4, 4,14, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 1, 0, 3, 0, 0, 1, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
};


static void MoveCamera(LinceCamera* cam, float ds){
    if(LinceIsKeyPressed(LinceKey_d)) cam->pos[0] +=  ds;
    if(LinceIsKeyPressed(LinceKey_a)) cam->pos[0] += -ds;
    if(LinceIsKeyPressed(LinceKey_w)) cam->pos[1] +=  ds;
    if(LinceIsKeyPressed(LinceKey_s)) cam->pos[1] += -ds;
}

static void UpdatePlayer(GameData* game_data){
    game_data->player_box.x = game_data->camera.pos[0];
    game_data->player_box.y = game_data->camera.pos[1];
    game_data->player_transform.x = game_data->camera.pos[0];
    game_data->player_transform.y = game_data->camera.pos[1];
}


void WorldSceneInit(LinceScene* scene){
    WorldScene* world_scene = LinceAlloc(sizeof(WorldScene));
    scene->data = world_scene;
    
    // Town map
    LinceTexture* tex = LinceAppGetAsset(string_scoped_lit("textures/outside.png"), string_scoped_lit("texture"));
    LinceTilesetInit(&world_scene->tileset, tex, 16, 16);
    
    world_scene->map =  (LinceTilemap){
        .width = 15, .height = 15,
        .scale = {1,1}, .pos = {-2,0},
    };
    LinceTilemapInit(&world_scene->map, OUTSIDE_GRID);
    LinceTilemapUseTileset(&world_scene->map, &world_scene->tileset);
 
    world_scene->house_door = (DoorLink){
        .box = (LinceBox2D){.x=6-0.5, .y=6-0.5, .w=1, .h=1},
        .to_scene = "House", .to_scene_len = sizeof("House") - 1,
        .to_x = 3, .to_y = 2,
    };

    // world_scene->door_link  = (LinceBox2D){.x=7-0.5, .y=6-0.5, .w=1, .h=1};
    // world_scene->player_box = (LinceBox2D){.x=0, .y=0, .w=0.7, .h=0.7};
    // world_scene->player_sprite = (LinceSprite){.x=0, .y=0, .w=0.7, .h=0.7, .color={0,0,1,1}, .zorder=1};
}

void WorldSceneUpdate(LinceScene* scene, float dt){
    WorldScene* world_scene = scene->data;
    GameData* game_data = LinceGetApp()->user_data;

    MoveCamera(&game_data->camera, dt * game_data->camera_speed);
    
    LinceBeginRender(&game_data->camera);
    LinceDrawTilemap(&world_scene->map, NULL);
    LinceDrawSprite(&game_data->player_sprite, &game_data->player_transform, NULL);
    UpdatePlayer(game_data);

    // Debug - show location of door link
    LinceDrawSprite(
        &(LinceSprite){
            .color = {1,0,0}, .zorder = 1,
            .alpha=0.5, .flags = LinceSprite_UseAlpha
        },
        &(LinceTransform){
            .x = world_scene->house_door.box.x,
            .y = world_scene->house_door.box.y,
            .w = world_scene->house_door.box.w,
            .h = world_scene->house_door.box.h,
        },
        NULL
    );

    // Wait for interact key to enter door
    if(LinceBox2DCollides(&game_data->player_box, &world_scene->house_door.box)){
        // "Press E to enter"
        if(LinceIsKeyPressed(LinceKey_e)){
            game_data->camera.pos[0] = world_scene->house_door.to_x;
            game_data->camera.pos[1] = world_scene->house_door.to_y;
            LinceAppLoadScene(string_scoped(world_scene->house_door.to_scene, world_scene->house_door.to_scene_len));
        }
    }

    LinceEndRender();
}

void WorldSceneDestroy(LinceScene* scene){
    WorldScene* world_scene = scene->data;
    LinceTilemapUninit(&world_scene->map);
    LinceTilesetUninit(&world_scene->tileset);
    LinceFree(scene->data);
}