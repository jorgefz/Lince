#ifndef LINCE_WINDOW_EVENT_H
#define LINCE_WINDOW_EVENT_H

#include "event/event.h"

typedef struct LinceWindowCloseEvent {
    int dummy;
} LinceWindowCloseEvent;

typedef struct LinceWindowResizeEvent {
    unsigned int height, width;
} LinceWindowResizeEvent;

/* Helper functions to create and allocate events */
LinceEvent LinceNewWindowCloseEvent();
LinceEvent LinceNewWindowResizeEvent(int width, int height);

#endif // LINCE_WINDOW_EVENT_H