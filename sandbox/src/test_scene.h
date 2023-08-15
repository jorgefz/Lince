#ifndef TEST_SCENE
#define TEST_SCENE

#include <lince.h>
#include "lince/scene/scene.h"


typedef struct BlueScene {
    LinceCamera camera;
} BlueScene;

typedef struct RedScene {
    LinceCamera camera;
} RedScene;


void InitBlueScene(LinceScene* scene);
void UninitBlueScene(LinceScene* scene);
void UpdateBlueScene(LinceScene* scene, float dt);
void DrawBlueScene(LinceScene* scene);

void InitRedScene(LinceScene* scene);
void UninitRedScene(LinceScene* scene);
void UpdateRedScene(LinceScene* scene, float dt);
void DrawRedScene(LinceScene* scene);


#endif /* TEST_SCENE */