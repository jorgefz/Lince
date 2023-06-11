#ifndef TEST_SCENE
#define TEST_SCENE

#include <lince.h>
#include "scene.h"


typedef struct BlueScene {
    int dummy;
} BlueScene;

typedef struct RedScene {
    int dummy;
} RedScene;


void InitBlueScene(LinceSceneStack* stack, LinceScene* scene);
void UninitBlueScene(LinceSceneStack* stack, LinceScene* scene);
void UpdateBlueScene(LinceSceneStack* stack, LinceScene* scene, float dt);
void DrawBlueScene(LinceSceneStack* stack, LinceScene* scene);

void InitRedScene(LinceSceneStack* stack, LinceScene* scene);
void UninitRedScene(LinceSceneStack* stack, LinceScene* scene);
void UpdateRedScene(LinceSceneStack* stack, LinceScene* scene, float dt);
void DrawRedScene(LinceSceneStack* stack, LinceScene* scene);


#endif /* TEST_SCENE */