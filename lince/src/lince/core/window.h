#ifndef LINCE_WINDOW_H
#define LINCE_WINDOW_H

#include "lince/event/event.h"

typedef void (*LinceEventCallbackFn)(LinceEvent*);

typedef struct LinceWindow {
    void* handle;           // glfw handle
    uint32_t height, width; // window size in pixels
    LinceBool initialised;  // 
    char title[LINCE_NAME_MAX];          // String displayed at top of window
    LinceEventCallbackFn event_callback; // Translates glfw events to LinceEvents
} LinceWindow; 

/* Initialises GLFW window */
LinceWindow* LinceCreateWindow(unsigned int width, unsigned int height, const char* title);

/* Signals whether the window should be shutdown */
unsigned int LinceShouldCloseWindow(LinceWindow* window);

/* Swaps buffers and polls GLFW events */
void LinceUpdateWindow(LinceWindow* window);

/* Shutds down window */
void LinceDestroyWindow(LinceWindow* window);

/* Set the function called every time an event occurs,
which then propagates it to other callbacks */
void LinceSetMainEventCallback(LinceWindow* window, LinceEventCallbackFn func);


#endif // LINCE_WINDOW_H