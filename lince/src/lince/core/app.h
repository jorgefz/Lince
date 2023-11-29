#ifndef LINCE_APP_H
#define LINCE_APP_H

/** @file app.h
* Main initialisation and execution functions for the Lince engine
*/
#include "cglm/mat4.h"
#include "lince/containers/array.h"
#include "lince/containers/hashmap.h"
#include "lince/core/window.h"
#include "lince/core/layer.h"
#include "lince/event/event.h"
#include "lince/event/key_event.h"
#include "lince/event/mouse_event.h"
#include "lince/event/window_event.h"
#include "lince/renderer/camera.h"
#include "lince/gui/ui_layer.h"
#include "lince/scene/scene.h"
#include "lince/asset_manager/asset_manager.h"

/** Function pointer typedefs for user-defined application callbacks */
typedef void (*LinceGameInitFn)();
typedef void (*LinceGameOnUpdateFn)(float dt);
typedef void (*LinceGameOnEventFn)(LinceEvent*);
typedef void (*LinceGameTerminateFn)();

/** @struct LinceApp
* Holds the application's state.
*/
typedef struct LinceApp{

    /* User callbacks */
    LinceGameInitFn      on_init;      ///< User-defined callback called once when the app is initialized.
    LinceGameOnUpdateFn  on_update;    ///< User-defined callback called once per frame.
    LinceGameOnEventFn   on_event;     ///< User-defined callback called when an event is propagated.
    LinceGameTerminateFn on_terminate; ///< User-defined callback called when the app closes.

    /* User settings */
    void* user_data;        ///< Pointer that may be freely re-assiged by the user.      
    uint32_t screen_width;  ///< Width in pixels of the window.
    uint32_t screen_height; ///< Height in pixels of the window.
    char* title;      ///< String displayed at the top of the window.

    /* Internal state */
    LinceWindow     *window;        ///< Window state.
    array_t layer_stack;   ///< Array of rendering layers.
    array_t overlay_stack; ///< Array of rendering overlays (drawn after layers).
    
    // array_t scene_stack; ///< Stack of scenes. Only topmost is rendered.
    hashmap_t scene_cache;   
    LinceScene* current_scene; ///< Scene at the top of the stack.

    LinceBool running;   ///< True if the application is active.
    float time_ms;       ///< Run time in milliseconds.
    float dt;            ///< Frame time step in milliseconds.
    int current_layer;   ///< Index of the active layer (during OnUpdate or OnEvent). This is `-1` if no layer is active.
    int current_overlay; ///< Index of the active overlay, akin to the current layer.
    
    LinceAssetManager asset_manager; ///< Finds paths to assets
    LinceUILayer* ui;   ///< State of the GUI, e.g. Nuklear's context.

} LinceApp;

/** @brief Runs main application loop. `LinceInit()` must have been called.
*/
void LinceRun();

/** @brief Fetches OpenGL errors and quits the program if any are found.
*/
void LinceCheckErrors();

/** @brief Adds a rendering layer to the application.
* @param layer Rendering layer to push onto the application's layer stack.
*/
void LincePushLayer(LinceLayer* layer);

/** @brief Adds a rendering overlay to the application.
* @param overlay Rendering overlays to push onto the application's overlay stack.
*                Overlays are rendered after layers.
*/
void LincePushOverlay(LinceLayer* overlay);

/** @brief Removes a rendering layer from the layer stack.
* @param layer Rendering layer to remove. It's 'on_detach' method will be called.
*/
void LincePopLayer(LinceLayer* layer);

/** @brief Removes a rendering overlay from the layer stack.
* @param layer Rendering overlay to remove. It's 'on_detach' method will be called.
*/
void LincePopOverlay(LinceLayer* overlay);


void LinceRegisterScene(const char* name, LinceScene* callbacks);

void LinceLoadScene(const char* name);


/** @brief Returns the global state of the application. See `LinceApp`.
*/
LinceApp* LinceGetApp();

/** @brief Returns aspect ratio of the window.
*/
float LinceGetAspectRatio();

/** @brief Returns the current window width and height in pixels.
* @param size Array of two floats with window width and height. 
*/
void LinceGetScreenSize(vec2 size);

/** @brief Transforms a point in space from screen to world coodinates.
* @param world_pos vec2 that returns the XY position in the world.
* @param screen_pos vec2 with the XY position in the screen to trasnform.
* @param camera Camera with an up-to-date inverse view-projection matrix
*/
void LinceTransformToWorld(vec2 world_pos, vec2 screen_pos, LinceCamera* camera);

/** @brief Transforms a point in space from world to screen coodinates.
* @param screen_pos vec2 that returns the XY position on screen.
* @param world_pos vec2 with the XY position in the world to transform.
* @param camera Camera with an up-to-date view-projection matrix
*/
void LinceTransformToScreen(vec2 screen_pos, vec2 world_pos, LinceCamera* camera);

/** @brief Returns the position of the mouse pointer in world coordinates.
* @param pos vec2 used to return the mouse position.
* @param cam Camera used to determine the world position.
* @todo pass the view-projection matrix instead of the camera.
*/
void LinceGetMousePosWorld(vec2 pos, LinceCamera* cam);

/* Transforms coordinates from world to screen coodinates */
// void LinceTransformToScreen();

/** @brief Returns current layer being handled or updated.
* Returns NULL if no layer is being handled.
* Should only be used within a layer's OnUpdate and OnEvent callbacks.
*/
LinceLayer* LinceGetCurrentLayer();

/** @brief Returns current overlay being handled or updated.
* Returns NULL if no overlay is being handled.
* Should only be used within a overlay's OnUpdate and OnEvent callbacks.
*/
LinceLayer* LinceGetCurrentOverlay();


#endif // LINCE_APP_H