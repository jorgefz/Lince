#ifndef LINCE_APP_H
#define LINCE_APP_H

/** @file app.h
* Main initialisation and execution functions for the Lince engine
*/

#include "lince/core/window.h"
#include "lince/core/layer.h"
#include "lince/event/event.h"
#include "lince/event/key_event.h"
#include "lince/event/mouse_event.h"
#include "lince/event/window_event.h"
#include "lince/renderer/camera.h"
#include "lince/gui/ui_layer.h"
#include "cglm/mat4.h"

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
    const char* title;      ///< String displayed at the top of the window.

    /* Internal state */
    LinceWindow     *window;        ///< Window state.
    LinceLayerStack *layer_stack;   ///< Array of rendering layers.
    LinceLayerStack *overlay_stack; ///< Array of rendering overlays (drawn after layers).
    LinceBool        running;       ///< True if the application is active.
    float time_ms;       ///< Run time in milliseconds.
    float dt;            ///< Frame time step in milliseconds.
    int current_layer;   ///< Index of the active layer (during OnUpdate or OnEvent). This is `-1` if no layer is active.
    int current_overlay; ///< Index of the active overlay, akin to the current layer.

    /* UI */
    LinceUILayer* ui;   ///< State of the GUI, e.g. Nuklear's context.

    LinceBool enable_profiling; ///< (unused)
    LinceBool enable_logging;   ///< (unused)
    char* profiler_filename;    ///< (unused)
    char* log_filename;         ///< (unused)
    FILE* log_file;             ///< (unused)
    FILE* profiler_file;        ///< (unused)

} LinceApp;

/** @brief Runs main application loop. `LinceInit()` must have been called.
*/
void LinceRun();

/** @brief Fetches OpenGL errors and quits the program if any are found.
*/
void LinceCheckErrors();

/** @brief Adds a rendering layer to the application.
* @param layer Rendering layer to push onto the application's layer stack.
*              Must have been initialised with `LinceCreateLayer`.
*/
void LincePushLayer(LinceLayer* layer);

/** @brief Adds a rendering overlay to the application.
* @param overlay Rendering overlays to push onto the application's overlay stack.
*                Overlays are rendered after layers.
*/
void LincePushOverlay(LinceLayer* overlay);

/** @brief Returns the application's run time in milliseconds.
*/
double LinceGetTimeMillis();

/** @brief Returns the global state of the application. See `LinceApp`.
*/
LinceApp* LinceGetAppState();

/** @brief Returns aspect ratio of the window.
*/
float LinceGetAspectRatio();

/** @brief Returns the current window width and height in pixels.
* @param size Array of two floats with window width and height. 
*/
void LinceGetScreenSize(vec2 size);

/** @brief Transforms a point in space from screen to world coodinates.
* @param screen_coords vec2 with the x and y position on screen, overwritten with the result.
* @param screen_size vec2 with the screen's width and height in pixels.
* @param vp_inv Inverse of the view-projection matrix (obtained from a camera).
* @todo Add extra argument for return vec2 world coordinates,
*       instead of overwriting `screen_coords`.
*/
void LinceTransformToWorld(vec2 screen_coords, vec2 screen_size, mat4 vp_inv);

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