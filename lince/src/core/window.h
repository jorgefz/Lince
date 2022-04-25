#ifndef LINCE_WINDOW_H
#define LINCE_WINDOW_H

#include "event/event.h"

typedef void (*LinceEventCallbackFn)(LinceEvent*);

typedef struct {
    void* handle;
    unsigned int height, width, initialised;
    char title[LINCE_STR_MAX];
    LinceEventCallbackFn event_callback;
} LinceWindow; 

/* Initialises GLFW window */
LinceWindow* LinceCreateWindow(unsigned int width, unsigned int height);

/* Signals whether the window should be shutdown */
unsigned int LinceShouldCloseWindow(LinceWindow* window);

/* Swaps buffers and polls GLFW events */
void LinceUpdateWindow(LinceWindow* window);

/* Shutds down window */
void LinceDestroyWindow(LinceWindow* window);

/* Set the function called every time an event occurs,
which then propagates it to other callbacks */
void LinceSetMainEventCallback(LinceWindow* window, LinceEventCallbackFn func);
// LinceSetMainEventCallback
// LinceSetEventCallback


#endif // LINCE_WINDOW_H