#ifndef WORLD_SCENE_H
#define WORLD_SCENE_H

#include <lince.h>
#include "lince/scene/scene.h"

typedef struct WorldScene {
    LinceCamera camera;
    LinceTilemap map;
} WorldScene;

void WorldSceneInit(LinceScene* scene);
void WorldSceneUpdate(LinceScene* scene, float dt);
void WorldSceneDestroy(LinceScene* scene);

#endif /* WORLD_SCENE_H */