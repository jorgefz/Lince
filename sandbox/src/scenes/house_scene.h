#ifndef HOUSE_H
#define HOUSE_H

#include <lince.h>

typedef struct HouseScene {
    LinceEntityRegistry* reg;
    LinceCamera camera;
    LinceTilemap map;
} HouseScene;

void HouseSceneInit(LinceScene* scene);
void HouseSceneUpdate(LinceScene* scene, float dt);
void HouseSceneDestroy(LinceScene* scene);

#endif /* HOUSE_H */