#ifndef LINCE_SCENE
#define LINCE_SCENE

#include <lince.h>

typedef struct LinceScene {
    // Data
    void* scene_data;
    char name[LINCE_NAME_MAX];

    // Callbacks
    void (*on_init)  (struct LinceScene*); // init custom data
    // void (*on_enter) (struct LinceScene*); // called when scene becomes active
    void (*on_update)(struct LinceScene*, float dt); //called on every frame
    void (*on_draw)  (struct LinceScene*); // called on draw
    // void (*on_event) (struct LinceScene*, LinceEvent*); // called on input event
    // void (*on_exit)  (struct LinceScene*); // called when scene becomes inactive
    void (*on_delete)(struct LinceScene*); // free custom data
} LinceScene;

typedef struct LinceSceneStack {
    array_t scenes;     // array<LinceScene>
    LinceScene* top;    // active scene
} LinceSceneStack;


void LinceInitScene(LinceScene* scene);

void LinceUninitScene(LinceScene* scene);

LinceSceneStack* LinceCreateSceneStack(void);

void LinceDestroySceneStack(LinceSceneStack* stack);

void LincePushScene(LinceSceneStack* stack, LinceScene* scene);

void LincePopScene(LinceSceneStack* stack);


#endif /* LINCE_SCENE */