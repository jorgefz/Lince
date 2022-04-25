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

LinceWindow* LinceWindow_Create(unsigned int width, unsigned int height);

unsigned int LinceWindow_ShouldClose(LinceWindow* window);

void LinceWindow_Update(LinceWindow* window);

void LinceWindow_Destroy(LinceWindow* window);

void LinceWindow_SetEventCallback(LinceWindow* window, LinceEventCallbackFn func);



#endif // LINCE_WINDOW_H