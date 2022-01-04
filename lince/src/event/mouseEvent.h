#ifndef LINCE_MOUSE_EVENT_H
#define LINCE_MOUSE_EVENT_H

#include "event/event.h"

typedef struct MouseButtonPressedEvent {
    int button;
} MouseButtonPressedEvent;

typedef struct MouseButtonReleasedEvent {
    int button;
} MouseButtonReleasedEvent;

typedef struct MouseMovedEvent {
    float xpos, ypos;
} MouseMovedEvent;

typedef struct MouseScrolledEvent {
    float xoff, yoff;
} MouseScrolledEvent;

LinceEvent LinceEvent_NewMouseButtonPressedEvent(int button);
LinceEvent LinceEvent_NewMouseButtonReleasedEvent(int button);
LinceEvent LinceEvent_NewMouseMovedEvent(double xpos, double ypos);
LinceEvent LinceEvent_NewMouseScrolledEvent(double xoff, double yoff);


#endif // LINCE_MOUSE_EVENT_H