#ifndef LINCE_SCENE
#define LINCE_SCENE

#include <lince.h>


/*

if (LinceStartScene(camera, shader)){
    LinceEndScene();
}

*/

struct LinceSceneStack;

typedef struct LinceScene {
    /* Data */
    void* data; // user data
    LinceCamera camera;
    // LinceEntityRegistry* entity_registry;

    /* Callbacks */
    // Called when loaded and pushed onto stack
    void (*on_init)  (struct LinceSceneStack* stack, struct LinceScene*);

    // Called when the scene is popped or the stack is destroyed
    void (*on_delete)(struct LinceSceneStack* stack, struct LinceScene*);

    // Called when it becomes active again (previous scene popped) 
    // void (*on_enter) (struct LinceScene*); // called when scene becomes active
    
    // Called when another scene is pushed onto the stack
    // void (*on_leave)  (struct LinceScene*); // called when scene becomes inactive

    // Called on every frame when active
    void (*on_update)(struct LinceSceneStack* stack, struct LinceScene*, float dt);
    
    // Called on every frame when active, after on_update.
    void (*on_draw)  (struct LinceSceneStack* stack, struct LinceScene*);
    
    // Called when an event is propagated
    // void (*on_event) (struct LinceScene*, LinceEvent*);

} LinceScene;

typedef struct LinceSceneStack {
    array_t scenes;     // array<LinceScene>
    LinceScene* top;    // active scene
} LinceSceneStack;


/** @brief Initialises scene components.
* @param stack Scene stack whose top item is the scene to initialise.
* @param scene Scene to initialise with desired callbacks defined.
*/
void LinceInitScene(LinceSceneStack* stack, LinceScene* scene);

void LinceUninitScene(LinceSceneStack* stack, LinceScene* scene);

LinceSceneStack* LinceCreateSceneStack(void);

void LinceDestroySceneStack(LinceSceneStack* stack);

void LincePushScene(LinceSceneStack* stack, LinceScene* scene);

void LincePopScene(LinceSceneStack* stack);

void LinceUpdateSceneStack(LinceSceneStack* stack, float dt);


#endif /* LINCE_SCENE */