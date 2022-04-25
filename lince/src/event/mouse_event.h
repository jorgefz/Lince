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

/* Helper functions to create and allocate new events */
LinceEvent LinceNewMouseButtonPressedEvent(int button);
LinceEvent LinceNewMouseButtonReleasedEvent(int button);
LinceEvent LinceNewMouseMovedEvent(double xpos, double ypos);
LinceEvent LinceNewMouseScrolledEvent(double xoff, double yoff);


#endif // LINCE_MOUSE_EVENT_H