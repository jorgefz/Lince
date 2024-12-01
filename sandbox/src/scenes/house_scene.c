
#include "house_scene.h"
#include "gamedata.h"

typedef enum HouseTilenames {
    House_Floor=0, House_Exit=1, House_Wall=2, House_WallL=3, House_WallR=4,
    House_TableCandle=5 
} HouseTilenames;

static uint32_t HOUSE_GRID[] = {
    3,15,16,15,16,15, 4,
    3, 2, 2, 2, 2, 9, 4,
    3,13,12, 0, 0,14, 4,
    3, 0, 0, 0, 0, 0, 4,
    3, 0, 0, 0,10,10, 4,
    3, 5, 0, 0, 7, 8, 4,
    3, 0, 0, 0, 0,11, 4,
   24,24,24, 1,24,24,24
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


void HouseSceneInit(LinceScene* scene){
    HouseScene* house_scene = LinceAlloc(sizeof(HouseScene));
    scene->data = house_scene;
    
    // Town map
    LinceTexture* tex = LinceAppGetAsset(string_scoped_lit("textures/inside.png"), string_scoped_lit("texture"));
    LinceTilesetInit(&house_scene->tileset, tex, 16, 16);
    
    house_scene->map =  (LinceTilemap){
        .width = 7, .height = 8,
        .scale = {1,1}, .pos = {-1,0},
    };
    LinceTilemapInit(&house_scene->map, HOUSE_GRID);
    LinceTilemapUseTileset(&house_scene->map, &house_scene->tileset);

    house_scene->house_door = (DoorLink){
        .box = (LinceBox2D){.x=3-0.5, .y=1-0.5, .w=1, .h=1},
        .to_scene = "World", .to_scene_len = sizeof("World") - 1,
        .to_x = 5.5,
        .to_y = 4.5,
    };
}

void HouseSceneUpdate(LinceScene* scene, float dt){
    HouseScene* house_scene = scene->data;
    GameData* game_data = LinceGetApp()->user_data;

    MoveCamera(&game_data->camera, dt * game_data->camera_speed);
    
    LinceBeginRender(&game_data->camera);
    LinceDrawTilemap(&house_scene->map, NULL);

    LinceDrawSprite(&game_data->player_sprite, &game_data->player_transform, NULL);
    UpdatePlayer(game_data);

    // Debug - show location of door link
    LinceDrawSprite(
        &(LinceSprite){
            .color = {1,0,0}, .zorder = 1,
            .alpha=0.5, .flags = LinceSprite_UseAlpha
        },
        &(LinceTransform){
            .x = house_scene->house_door.box.x,
            .y = house_scene->house_door.box.y,
            .w = house_scene->house_door.box.w,
            .h = house_scene->house_door.box.h,
        },
        NULL
    );


    LinceEndRender();

    // Wait for interact key to enter door
   if(LinceBox2DCollides(&game_data->player_box, &house_scene->house_door.box)){
        // "Press E to enter"
        if(LinceIsKeyPressed(LinceKey_e)){
            game_data->camera.pos[0] = house_scene->house_door.to_x;
            game_data->camera.pos[1] = house_scene->house_door.to_y;
            LinceAppLoadScene(string_scoped(house_scene->house_door.to_scene, house_scene->house_door.to_scene_len));
        }
    }
}

void HouseSceneDestroy(LinceScene* scene){
    HouseScene* house_scene = scene->data;
    LinceTilemapUninit(&house_scene->map);
    LinceTilesetUninit(&house_scene->tileset);
    LinceFree(scene->data);
}