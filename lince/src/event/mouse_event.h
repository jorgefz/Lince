#ifndef LINCE_MOUSE_EVENT_H
#define LINCE_MOUSE_EVENT_H

#include "event/event.h"

typedef struct LinceMouseButtonPressedEvent {
    int button;
} LinceMouseButtonPressedEvent;

typedef struct LinceMouseButtonReleasedEvent {
    int button;
} LinceMouseButtonReleasedEvent;

typedef struct LinceMouseMovedEvent {
    float xpos, ypos;
} LinceMouseMovedEvent;

typedef struct LinceMouseScrolledEvent {
    float xoff, yoff;
} LinceMouseScrolledEvent;

/* Helper functions to create and allocate new events */
LinceEvent LinceNewMouseButtonPressedEvent(int button);
LinceEvent LinceNewMouseButtonReleasedEvent(int button);
LinceEvent LinceNewMouseMovedEvent(double xpos, double ypos);
LinceEvent LinceNewMouseScrolledEvent(double xoff, double yoff);


#endif // LINCE_MOUSE_EVENT_H