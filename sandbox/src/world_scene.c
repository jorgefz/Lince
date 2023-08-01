#include "world_scene.h"


static void MoveCamera(LinceCamera* cam, float dt){

    static const float vel = 8e-4;

    if(LinceIsKeyPressed(LinceKey_d)) cam->pos[0] +=  vel * dt;
    if(LinceIsKeyPressed(LinceKey_a)) cam->pos[0] += -vel * dt;
    if(LinceIsKeyPressed(LinceKey_w)) cam->pos[1] +=  vel * dt;
    if(LinceIsKeyPressed(LinceKey_s)) cam->pos[1] += -vel * dt;

}


void WorldSceneInit(LinceSceneStack* stack, LinceScene* scene){
    WorldScene* world_scene = LinceMalloc(sizeof(WorldScene));
    scene->data = world_scene;
    // Load town map
    world_scene->map =  (LinceTilemap){
        .texture = LinceLoadTexture(
            "sandbox/assets/textures/bricks.png", LinceTexture_FlipY),
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
    LinceInitTilemap(&world_scene->map);
    
}

void WorldSceneUpdate(LinceSceneStack* stack, LinceScene* scene, float dt){
    WorldScene* world_scene = scene->data;

    MoveCamera(&scene->camera, dt);
    LinceResizeCameraView(&scene->camera, LinceGetAspectRatio());
    LinceUpdateCamera(&scene->camera);
    
    LinceBeginScene(&scene->camera);
    LinceDrawTilemap(&world_scene->map, NULL);
    LinceEndScene();
}

void WorldSceneDestroy(LinceSceneStack* stack, LinceScene* scene){
    WorldScene* world_scene = scene->data;
    LinceUninitTilemap(&world_scene->map);
    LinceFree(scene->data);
}