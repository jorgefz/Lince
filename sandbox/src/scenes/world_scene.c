#include "world_scene.h"

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
    0, 0, 0, 0, 0,18,11,11,19, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0,10,11,11,12, 0, 0, 0, 0, 0, 0,
    1, 0, 1, 0, 0,10,11,15,12, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0,
    1, 0, 0,13, 4, 4, 0, 4, 4, 4,14, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 1, 0, 4, 0, 0, 1, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
};


static void MoveCamera(LinceCamera* cam, float dt){
    static const float vel = 8e-4;
    if(LinceIsKeyPressed(LinceKey_d)) cam->pos[0] +=  vel * dt;
    if(LinceIsKeyPressed(LinceKey_a)) cam->pos[0] += -vel * dt;
    if(LinceIsKeyPressed(LinceKey_w)) cam->pos[1] +=  vel * dt;
    if(LinceIsKeyPressed(LinceKey_s)) cam->pos[1] += -vel * dt;

    if(LinceIsKeyPressed(LinceKey_Space)){
        LincePushScene(hashmap_get(LinceGetApp()->user_data, "House"));
    }
}


void WorldSceneInit(LinceScene* scene){
    WorldScene* world_scene = LinceMalloc(sizeof(WorldScene));
    scene->data = world_scene;
    
    // Town map
    world_scene->map =  (LinceTilemap){
        .texture = LinceLoadTexture(
            "sandbox/assets/textures/outside.png",
            LinceTexture_FlipY
        ),
        .cellsize = {16,16},
        .scale = {0.3,0.3},
        .offset = {-1, -1},
        .width = 15,
        .height = 15,
        .grid = OUTSIDE_GRID
    };
    LinceInitTilemap(&world_scene->map);
    LinceInitCamera(&world_scene->camera, LinceGetAspectRatio());
}

void WorldSceneUpdate(LinceScene* scene, float dt){
    WorldScene* world_scene = scene->data;

    MoveCamera(&world_scene->camera, dt);
    LinceResizeCameraView(&world_scene->camera, LinceGetAspectRatio());
    LinceUpdateCamera(&world_scene->camera);
    
    LinceBeginScene(&world_scene->camera);
    LinceDrawTilemap(&world_scene->map, NULL);
    LinceEndScene();
}

void WorldSceneDestroy(LinceScene* scene){
    WorldScene* world_scene = scene->data;
    LinceUninitTilemap(&world_scene->map);
    LinceFree(scene->data);
}