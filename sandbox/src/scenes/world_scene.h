#ifndef WORLD_SCENE_H
#define WORLD_SCENE_H

#include <lince.h>
#include "gamedata.h"

typedef struct WorldScene {
    LinceEntityRegistry* reg;
    LinceCamera camera;
    LinceTilemap map;
    DoorLink house_door;
} WorldScene;

void WorldSceneInit(LinceScene* scene);
void WorldSceneUpdate(LinceScene* scene, float dt);
void WorldSceneDestroy(LinceScene* scene);

#endif /* WORLD_SCENE_H */