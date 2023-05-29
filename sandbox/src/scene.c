#include "scene.h"
#include "components.h"

void LinceInitScene(LinceScene* scene){
    if(scene->on_init){
        scene->on_init(scene);
    }
}

void LinceUninitScene(LinceScene* scene){
    if(scene->on_delete){
        scene->on_delete(scene);
    }
}

LinceSceneStack* LinceCreateSceneStack(){
    LinceSceneStack* stack = LinceMalloc(sizeof(LinceSceneStack));
    array_init(&stack->scenes, sizeof(LinceScene));
    stack->top = NULL;
    return stack;
}

void LinceDestroySceneStack(LinceSceneStack* stack){
    while(stack->scenes.size > 0){
        LincePopScene(stack);
    }
    array_uninit(&stack->scenes);
    LinceFree(stack);
}

void LincePushScene(LinceSceneStack* stack, LinceScene* scene){
    LINCE_ASSERT(stack, "NULL argument");
    array_push_back(&stack->scenes, scene);
    stack->top = array_back(&stack->scenes);
    LinceInitScene(stack->top);
}

void LincePopScene(LinceSceneStack* stack){
    LINCE_ASSERT(stack && stack->top, "NULL argument");
    LinceUninitScene(stack->top);
    array_pop_back(&stack->scenes);
    stack->top = array_back(&stack->scenes);
}