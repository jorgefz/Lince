#ifndef WORLD_SCENE_H
#define WORLD_SCENE_H

#include <lince.h>
#include "scene.h"

typedef struct WorldScene {
    LinceTilemap map;
} WorldScene;

void WorldSceneInit(LinceSceneStack* stack, LinceScene* scene);
void WorldSceneUpdate(LinceSceneStack* stack, LinceScene* scene, float dt);
void WorldSceneDestroy(LinceSceneStack* stack, LinceScene* scene);

#endif /* WORLD_SCENE_H */