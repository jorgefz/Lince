
#include "event/mouse_event.h"

LinceEvent LinceNewMouseButtonPressedEvent(int button){
    LinceEvent e = {
        .type = LinceEventType_MouseButtonPressed,
        .name = "LinceMouseButtonPressedEvent",
        .handled = 0,
        .data = {0}
    };
    LinceMouseButtonPressedEvent m = {.button = button};
    e.data.MouseButtonPressed = malloc(sizeof(LinceMouseButtonPressedEvent));
    LINCE_ASSERT(e.data.MouseButtonPressed, "Failed to allocate memory");
    *(e.data.MouseButtonPressed) = m;
    return e;
}

LinceEvent LinceNewMouseButtonReleasedEvent(int button){
    LinceEvent e = {
        .type = LinceEventType_MouseButtonReleased,
        .name = "LinceMouseButtonReleasedEvent",
        .handled = 0,
        .data = {0}
    };
    LinceMouseButtonReleasedEvent m = {.button = button};
    e.data.MouseButtonReleased = malloc(sizeof(LinceMouseButtonReleasedEvent));
    LINCE_ASSERT(e.data.MouseButtonReleased, "Failed to allocate memory");
    *(e.data.MouseButtonReleased) = m;
    return e;
}

LinceEvent LinceNewMouseMovedEvent(double xpos, double ypos){
    LinceEvent e = {
        .type = LinceEventType_MouseMoved,
        .name = "LinceMouseMovedEvent",
        .handled = 0,
        .data = {0}
    };
    LinceMouseMovedEvent m = {.xpos = (float)xpos, .ypos = (float)ypos};
    e.data.MouseMoved = malloc(sizeof(LinceMouseMovedEvent));
    LINCE_ASSERT(e.data.MouseMoved, "Failed to allocate memory");
    *(e.data.MouseMoved) = m;
    return e;
}

LinceEvent LinceNewMouseScrolledEvent(double xoff, double yoff){
    LinceEvent e = {
        .type = LinceEventType_MouseScrolled,
        .name = "LinceMouseScrolledEvent",
        .handled = 0,
        .data = {0}
    };
    LinceMouseScrolledEvent m = {.xoff = (float)xoff, .yoff = (float)yoff};
    e.data.MouseScrolled = malloc(sizeof(LinceMouseScrolledEvent));
    LINCE_ASSERT(e.data.MouseScrolled, "Failed to allocate memory");
    *(e.data.MouseScrolled) = m;
    return e;
}