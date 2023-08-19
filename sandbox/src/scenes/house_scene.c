
#include "house_scene.h"

typedef enum HouseTilenames {
    House_Floor=0, House_Exit=1, House_Wall=2, House_WallL=3, House_WallR=4,
    House_TableCandle=5 
} HouseTilenames;

static uint32_t HOUSE_GRID[] = {
    3,2,2,2,4,
    3,0,0,0,4,
    3,0,5,0,4,
    3,0,0,0,4,
    6,6,1,6,6
};

static void MoveCamera(LinceCamera* cam, float dt){
    static const float vel = 8e-4;
    if(LinceIsKeyPressed(LinceKey_d)) cam->pos[0] +=  vel * dt;
    if(LinceIsKeyPressed(LinceKey_a)) cam->pos[0] += -vel * dt;
    if(LinceIsKeyPressed(LinceKey_w)) cam->pos[1] +=  vel * dt;
    if(LinceIsKeyPressed(LinceKey_s)) cam->pos[1] += -vel * dt;

}

void HouseSceneInit(LinceScene* scene){
    HouseScene* house_scene = LinceMalloc(sizeof(HouseScene));
    scene->data = house_scene;
    
    // Town map
    house_scene->map =  (LinceTilemap){
        .texture = LinceLoadTexture(
            "sandbox/assets/textures/inside.png",
            LinceTexture_FlipY
        ),
        .cellsize = {16,16},
        .scale = {0.3,0.3},
        .offset = {-1, -1},
        .width = 5,
        .height = 5,
        .grid = HOUSE_GRID
    };
    LinceInitTilemap(&house_scene->map);
    LinceInitCamera(&house_scene->camera, LinceGetAspectRatio());
}

void HouseSceneUpdate(LinceScene* scene, float dt){
    HouseScene* house_scene = scene->data;

    MoveCamera(&house_scene->camera, dt);
    LinceResizeCameraView(&house_scene->camera, LinceGetAspectRatio());
    LinceUpdateCamera(&house_scene->camera);
    
    LinceBeginScene(&house_scene->camera);
    LinceDrawTilemap(&house_scene->map, NULL);
    LinceEndScene();
}

void HouseSceneDestroy(LinceScene* scene){
    HouseScene* house_scene = scene->data;
    LinceUninitTilemap(&house_scene->map);
    LinceFree(scene->data);
}