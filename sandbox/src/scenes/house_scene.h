#ifndef HOUSE_H
#define HOUSE_H

#include <lince.h>
#include "gamedata.h"

typedef struct HouseScene {
    LinceEntityRegistry* reg;
    LinceCamera camera;
    LinceTileset tileset;
    LinceTilemap map;
    DoorLink house_door;
} HouseScene;

void HouseSceneInit(LinceScene* scene);
void HouseSceneUpdate(LinceScene* scene, float dt);
void HouseSceneDestroy(LinceScene* scene);

#endif /* HOUSE_H */