#include "lince/scene/scene.h"

void LinceInitScene(LinceScene* scene){
    scene->initialised = LinceTrue;
    if(scene->on_init){
        scene->on_init(scene);
    }
}

void LinceUninitScene(LinceScene* scene){
    if(scene->on_delete){
        scene->on_delete(scene);
    }
    scene->initialised = LinceFalse;
}

void LinceUpdateScene(LinceScene* scene, float dt){
    if(scene->on_update){
        scene->on_update(scene, dt);
    }
    if(scene->on_draw){
        scene->on_draw(scene);
    }
}