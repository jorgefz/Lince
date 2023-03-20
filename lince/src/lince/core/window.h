#ifndef LINCE_WINDOW_H
#define LINCE_WINDOW_H

#include "lince/event/event.h"

/** @typedef LinceEventCallbackFn
* @brief Simplifies pointer type for function that takes a LinceEvent.
*/
typedef void (*LinceEventCallbackFn)(LinceEvent*);

/** @struct LinceWindow
* @brief Holds the state of a window object
*/
typedef struct LinceWindow {
    void* handle;                        ///< GLFW handle
    uint32_t height, width;              ///< Window size in pixels
    LinceBool initialised;               ///< True if the window is currently active 
    char title[LINCE_NAME_MAX];          ///< String displayed at top of window
    LinceEventCallbackFn event_callback; ///< Translates GLFW events into LinceEvents
} LinceWindow; 

/** @brief Initialises GLFW window
* @param width Width in pixels of the window
* @param height Height in pixels of the window
* @param title String text shown at the top of the window
* @todo Add third argument for bit flags for fullscreen, vsync, etc
*/
LinceWindow* LinceCreateWindow(uint32_t width, uint32_t height, const char* title);

/** @brief Signals whether the window should be shutdown */
uint32_t LinceShouldCloseWindow(LinceWindow* window);

/** @brief Swaps buffers and polls GLFW events */
void LinceUpdateWindow(LinceWindow* window);

/** @brief Shutds down window */
void LinceDestroyWindow(LinceWindow* window);

/** @brief Set the function called every time an event occurs,
which then propagates it to other callbacks */
void LinceSetMainEventCallback(LinceWindow* window, LinceEventCallbackFn func);


#endif // LINCE_WINDOW_H