
#include "event/mouseEvent.h"

LinceEvent LinceEvent_NewMouseButtonPressedEvent(int button){
    LinceEvent e = {
        .type = LinceEventType_MouseButtonPressed,
        .name = "MouseButtonPressedEvent",
        .handled = 0,
        .data = {0}
    };
    MouseButtonPressedEvent m = {.button = button};
    e.data.MouseButtonPressed = malloc(sizeof(MouseButtonPressedEvent));
    LINCE_ASSERT(e.data.MouseButtonPressed, "Failed to allocate memory");
    *(e.data.MouseButtonPressed) = m;
    return e;
}

LinceEvent LinceEvent_NewMouseButtonReleasedEvent(int button){
    LinceEvent e = {
        .type = LinceEventType_MouseButtonReleased,
        .name = "MouseButtonReleasedEvent",
        .handled = 0,
        .data = {0}
    };
    MouseButtonReleasedEvent m = {.button = button};
    e.data.MouseButtonReleased = malloc(sizeof(MouseButtonReleasedEvent));
    LINCE_ASSERT(e.data.MouseButtonReleased, "Failed to allocate memory");
    *(e.data.MouseButtonReleased) = m;
    return e;
}

LinceEvent LinceEvent_NewMouseMovedEvent(double xpos, double ypos){
    LinceEvent e = {
        .type = LinceEventType_MouseMoved,
        .name = "MouseMovedEvent",
        .handled = 0,
        .data = {0}
    };
    MouseMovedEvent m = {.xpos = (float)xpos, .ypos = (float)ypos};
    e.data.MouseMoved = malloc(sizeof(MouseMovedEvent));
    LINCE_ASSERT(e.data.MouseMoved, "Failed to allocate memory");
    *(e.data.MouseMoved) = m;
    return e;
}

LinceEvent LinceEvent_NewMouseScrolledEvent(double xoff, double yoff){
    LinceEvent e = {
        .type = LinceEventType_MouseScrolled,
        .name = "MouseScrolledEvent",
        .handled = 0,
        .data = {0}
    };
    MouseScrolledEvent m = {.xoff = (float)xoff, .yoff = (float)yoff};
    e.data.MouseScrolled = malloc(sizeof(MouseScrolledEvent));
    LINCE_ASSERT(e.data.MouseScrolled, "Failed to allocate memory");
    *(e.data.MouseScrolled) = m;
    return e;
}