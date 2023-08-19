#ifndef SCENES_H
#define SCENES_H

#include "menu_scene.h"
#include "world_scene.h"
#include "house_scene.h"

typedef enum Scenes{
    Scene_MainMenu,
    Scene_World,
    Scene_House,

    Scene_Count
} Scenes;

LinceScene SCENE_CALLBACKS[] = {
    {
        .on_draw = DrawMainMenu,
    },
    {
        .on_init   = WorldSceneInit,
        .on_delete = WorldSceneDestroy,
        .on_update = WorldSceneUpdate,
    },
    {
        .on_init   = HouseSceneInit,
        .on_delete = HouseSceneDestroy,
        .on_update = HouseSceneUpdate,
    }
};


#endif /* SCENES_H */