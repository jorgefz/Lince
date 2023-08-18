#ifndef SCENES_H
#define SCENES_H

#include "menu_scene.h"
#include "world_scene.h"

typedef enum Scenes{
    Scene_MainMenu,
    Scene_World,

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
    }
};


#endif /* SCENES_H */