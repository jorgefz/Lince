#ifndef LINCE_WINDOW_H
#define LINCE_WINDOW_H

#include "event/event.h"

typedef void (*EventCallbackFn)(Event*);

typedef struct {
    void* handle;
    unsigned int height, width, initialised;
    char title[LINCE_STR_MAX];
    EventCallbackFn event_callback;
} LinceWindow; 

LinceWindow* LinceWindow_Create(unsigned int width, unsigned int height);

unsigned int LinceWindow_ShouldClose(LinceWindow* window);

void LinceWindow_Update(LinceWindow* window);

void LinceWindow_Destroy(LinceWindow* window);

void LinceWindow_SetEventCallback(LinceWindow* window, EventCallbackFn func);



#endif // LINCE_WINDOW_H