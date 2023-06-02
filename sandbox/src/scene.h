#ifndef LINCE_SCENE
#define LINCE_SCENE

#include <lince.h>

struct LinceSceneStack;

typedef struct LinceScene {
    // Data
    void* data;
    char name[LINCE_NAME_MAX];

    // Callbacks
    void (*on_init)  (struct LinceSceneStack* stack, struct LinceScene*);
    // void (*on_enter) (struct LinceScene*); // called when scene becomes active
    void (*on_update)(struct LinceSceneStack* stack, struct LinceScene*, float dt);
    void (*on_draw)  (struct LinceSceneStack* stack, struct LinceScene*);
    // void (*on_event) (struct LinceScene*, LinceEvent*);
    // void (*on_exit)  (struct LinceScene*); // called when scene becomes inactive
    void (*on_delete)(struct LinceSceneStack* stack, struct LinceScene*);
} LinceScene;

typedef struct LinceSceneStack {
    array_t scenes;     // array<LinceScene>
    LinceScene* top;    // active scene
} LinceSceneStack;


void LinceInitScene(LinceSceneStack* stack, LinceScene* scene);

void LinceUninitScene(LinceSceneStack* stack, LinceScene* scene);

LinceSceneStack* LinceCreateSceneStack(void);

void LinceDestroySceneStack(LinceSceneStack* stack);

void LincePushScene(LinceSceneStack* stack, LinceScene* scene);

void LincePopScene(LinceSceneStack* stack);


#endif /* LINCE_SCENE */