#ifndef LINCE_SCENE
#define LINCE_SCENE

#include "lince/core/core.h"

/*
if (LinceStartScene(camera, shader)){
    LinceEndScene();
}
*/

typedef struct LinceScene {
    /* Data */
    void* data; // user data
    // LinceEntityRegistry* entity_registry;

    // Set to true when on_init method has been called
    LinceBool initialised;

    /* Callbacks */
    // Called when loaded and pushed onto stack
    void (*on_init)  (struct LinceScene*);

    // Called when the scene is popped or the stack is destroyed
    void (*on_delete)(struct LinceScene*);

    // Called when it becomes active again (previous scene popped) 
    // void (*on_enter) (struct LinceScene*); // called when scene becomes active
    
    // Called when another scene is pushed onto the stack
    // void (*on_leave)  (struct LinceScene*); // called when scene becomes inactive

    // Called on every frame when active
    void (*on_update)(struct LinceScene*, float dt);
    
    // Called on every frame when active, after on_update.
    void (*on_draw)  (struct LinceScene*);
    
    // void (*on_event) (struct LinceScene*, LinceEvent*);

    /// Serialize scene
    // void (*on_serial) (struct LinceScene*);

    /// Deseralize scene
    // void (*on_load) (struct LinceScene*);

} LinceScene;


/** @brief Initialises scene components.
* @param scene Scene to initialise with desired callbacks defined.
*/
void LinceInitScene(LinceScene* scene);

/** @brief Uninitialises scene components.
* @param scene Scene to uninitialise.
*/
void LinceUninitScene(LinceScene* scene);

/** @brief Updates scene logic and draws its geometry.
* @param scene Scene to update and draw.
*/
void LinceUpdateScene(LinceScene* scene, float dt);


#endif /* LINCE_SCENE */