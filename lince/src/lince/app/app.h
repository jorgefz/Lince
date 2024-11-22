#ifndef LINCE_APP_H
#define LINCE_APP_H

/** @file app.h
* Main initialisation and execution functions for the Lince engine
*/
#include "cglm/mat4.h"
#include "lince/containers/array.h"
#include "lince/containers/str.h"
#include "lince/containers/hashmap.h"
#include "lince/utils/clock.h"
#include "lince/core/window.h"
#include "lince/app/layer.h"
#include "lince/event/event.h"
#include "lince/event/key_event.h"
#include "lince/event/mouse_event.h"
#include "lince/event/window_event.h"
#include "lince/renderer/camera.h"
#include "lince/gui/gui.h"
#include "lince/scene/scene.h"
#include "lince/app/asset_cache.h"
#include "lince/renderer/transform.h"
#include "lince/input/input.h"

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
    string_t title;              ///< String displayed at the top of the window.
    void* user_data;             ///< Pointer that may be freely re-assiged by the user.      
    uint32_t screen_width;       ///< Width in pixels of the window.
    uint32_t screen_height;      ///< Height in pixels of the window.
    LinceBool show_debug_panel;  ///< Shows UI panel with debug info

    /* Internal state */
    LinceWindow     *window;        ///< Window state.
    array_t layer_stack;   ///< Array of rendering layers.
    array_t overlay_stack; ///< Array of rendering overlays (drawn after layers).
    
    // array_t scene_stack; ///< Stack of scenes. Only topmost is rendered.
    hashmap_t scene_cache;
    LinceScene* current_scene; ///< Scene at the top of the stack.

    LinceBool running;   ///< True if the application is active.
    LinceClock clock;    ///< Application timer
    float runtime;       ///< Run time in milliseconds.
    float dt;            ///< Frame time step in milliseconds.
    
    int current_layer;   ///< Index of the active layer (during OnUpdate or OnEvent). This is `-1` if no layer is active.
    int current_overlay; ///< Index of the active overlay, akin to the current layer.
    
    LinceAssetCache asset_cache; ///< Caches data loaded from disk
    LinceUI* ui;   ///< State of the GUI, e.g. Nuklear's context.

} LinceApp;

/** @brief Runs main application loop. `LinceInit()` must have been called.
*/
void LinceRun();

/** @brief Returns the global state of the application. See `LinceApp`.
*/
LinceApp* LinceGetApp(void);

/** @brief Set the window title. Only works before the window is initialised.
 * @note If the length of the title exceeeds LINCE_TITLE_MAX, it will be truncated to that value.
 * @param title Title string
 * @param len Number of characters in the title
*/
void LinceAppSetTitle(const char* title, size_t len);

/** @brief Retrieve the asset cache of the application */
LinceAssetCache* LinceAppGetAssetCache();

/** @brief Save the location of an assets folder relative to the executable */
void LinceAppPushAssetFolder(const char* dir);

/** @brief Retrieve (or load) an asset from the cache */
void* LinceAppGetAsset(const char* path, const char* type);

// hashmap_t* LinceAppGetShaderHeaders(); // For creating preprocessing shaders

/** @brief Adds a rendering layer to the application.
* @param layer Rendering layer to push onto the application's layer stack.
*/
void LinceAppPushLayer(LinceLayer* layer);

/** @brief Adds a rendering overlay to the application.
* @param overlay Rendering overlays to push onto the application's overlay stack.
*                Overlays are rendered after layers.
*/
void LinceAppPushOverlay(LinceLayer* overlay);

/** @brief Removes a rendering layer from the layer stack.
* @param layer Rendering layer to remove. It's 'on_detach' method will be called.
*/
void LinceAppPopLayer(LinceLayer* layer);

/** @brief Removes a rendering overlay from the layer stack.
* @param layer Rendering overlay to remove. It's 'on_detach' method will be called.
*/
void LinceAppPopOverlay(LinceLayer* overlay);

/** @brief Returns current layer being handled or updated.
* Returns NULL if no layer is being handled.
* Should only be used within a layer's OnUpdate and OnEvent callbacks.
*/
LinceLayer* LinceAppGetCurrentLayer();

/** @brief Returns current overlay being handled or updated.
* Returns NULL if no overlay is being handled.
* Should only be used within a overlay's OnUpdate and OnEvent callbacks.
*/
LinceLayer* LinceAppGetCurrentOverlay();

/** @brief Creates new scene in cache with defined callbacks. Will not call `on_init`.
* @param name Name to assign to scene
* @param len Length of scene string name
* @param callbacks scene struct with callbacks defined
*/
void LinceAppRegisterScene(const char* name, size_t len, LinceScene* callbacks);

/** @brief Sets a scene as the current scene. Calls its on_init method if uninitialised.
* Must have been registered with `LinceRegisterScene`.
* @param name Name of scene to load
* @param len Length of scene string name
*/
void LinceAppLoadScene(const char* name, size_t len);

/** @brief Return the scene with a given string identifier, or NULL if the scene has not been registered.
* @param name Scene identifier to load
* @param len Length of scene string name
* @returns Scene with matching identifier
*/
LinceScene* LinceAppGetScene(const char* name, size_t len);

/** @brief Returns aspect ratio of the window.
*/
float LinceAppGetAspectRatio(void);

/** @brief Returns the current window width and height in pixels */
LincePoint LinceAppGetScreenSize(void);

/** @brief Returns the position of the mouse pointer in world coordinates.
* @param cam Camera used to determine the world position.
*/
LincePoint LinceGetMousePosWorld(LinceCamera* cam);




#endif // LINCE_APP_H