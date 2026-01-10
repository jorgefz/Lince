#ifndef LINCE_WINDOW_H
#define LINCE_WINDOW_H

#include "lince/core/core.h"
#include "lince/event/event.h"

/** @typedef LinceEventCallbackFn
* @brief Simplifies pointer type for function that takes a LinceEvent.
*/
typedef void (*LinceEventCallbackFn)(LinceEvent*);

/** @struct LinceWindowAttributes
 * @brief Holds configuration and attributes of a window object,
 * e.g. window size, video modes, etc.
 */
typedef struct LinceWindowAttributes {
    string_t title;         ///< Window title shown at the top bar
    uint32_t height, width; ///< Window dimensions in pixels when not in fullscreen mode
    LinceBool fullscreen;   ///< Whether window is in fullscreen mode
    LinceBool resizeable;   ///< Whether window can be resized
    LinceBool vsync;        ///< Whether VSync enabled
} LinceWindowAttributes;

/** @struct LinceWindow
* @brief Holds the state of a window object
*/
typedef struct LinceWindow {
    void* handle;                        ///< GLFW handle
    LinceBool initialised;               ///< True if the window is currently active 
    LinceEventCallbackFn event_callback; ///< Translates GLFW events into LinceEvents
    LinceWindowAttributes attrib;        ///< Window attributes, e.g. size
} LinceWindow; 

/** @brief Initialises GLFW window
* @param attrib Window attributes. 
*/
LinceWindow* LinceCreateWindow(LinceWindowAttributes config);

/** @brief Signals whether the window should be shutdown */
uint32_t LinceShouldCloseWindow(LinceWindow* window);

/** @brief Swaps buffers and polls GLFW events */
void LinceUpdateWindow(LinceWindow* window);

/** @brief Shutds down window */
void LinceDestroyWindow(LinceWindow* window);

/** @brief Set the function called every time an event occurs,
which then propagates it to other callbacks */
void LinceSetMainEventCallback(LinceWindow* window, LinceEventCallbackFn func);

/** @brief Change the window's fullscreen mode
to enabled (fullscreen = LinceTrue) or disabled (fullscreen = False) */
// void LinceSetWindowFullscreen(LinceWindow* window, LinceBool fullscreen);

/** @brief Change the window's fullscreen mode
to enabled (fullscreen = LinceTrue) or disabled (fullscreen = False) */
// void LinceIsWindowFullscreen(LinceWindow* window, LinceBool fullscreen);

/** @brief Enable syncing window's refresh rate to that of the screen */
void LinceSetWindowVSync(LinceWindow* window, LinceBool vsync);


#endif // LINCE_WINDOW_H