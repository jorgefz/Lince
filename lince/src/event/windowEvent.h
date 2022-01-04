#ifndef LINCE_WINDOW_EVENT_H
#define LINCE_WINDOW_EVENT_H

#include "event/event.h"

typedef struct WindowCloseEvent {
    int dummy;
} WindowCloseEvent;

typedef struct WindowResizeEvent {
    unsigned int height, width;
} WindowResizeEvent;


LinceEvent LinceEvent_NewWindowCloseEvent();
LinceEvent LinceEvent_NewWindowResizeEvent(int width, int height);

#endif // LINCE_WINDOW_EVENT_H