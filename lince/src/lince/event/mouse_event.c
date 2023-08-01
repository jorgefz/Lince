
#include "event/mouse_event.h"
#include "core/memory.h"

LinceEvent LinceNewMousePressEvent(int button){
    LinceEvent e = {
        .type = LinceEventType_MousePress,
        .name = "LinceMousePressEvent",
        .handled = 0,
        .data = {0}
    };
    LinceMousePressEvent m = {.button = button};
    e.data.mouse_press = LinceNewCopy(&m, sizeof(LinceMousePressEvent));
    return e;
}

LinceEvent LinceNewMouseReleaseEvent(int button){
    LinceEvent e = {
        .type = LinceEventType_MouseRelease,
        .name = "LinceMouseReleaseEvent",
        .handled = 0,
        .data = {0}
    };
    LinceMouseReleaseEvent m = {.button = button};
    e.data.mouse_release = LinceNewCopy(&m, sizeof(LinceMouseReleaseEvent));
    return e;
}

LinceEvent LinceNewMouseMoveEvent(double xpos, double ypos){
    LinceEvent e = {
        .type = LinceEventType_MouseMove,
        .name = "LinceMouseMoveEvent",
        .handled = 0,
        .data = {0}
    };
    LinceMouseMoveEvent m = {.xpos = (float)xpos, .ypos = (float)ypos};
    e.data.mouse_move = LinceNewCopy(&m, sizeof(LinceMouseMoveEvent));
    return e;
}

LinceEvent LinceNewMouseScrollEvent(double xoff, double yoff){
    LinceEvent e = {
        .type = LinceEventType_MouseScroll,
        .name = "LinceMouseScrollEvent",
        .handled = 0,
        .data = {0}
    };
    LinceMouseScrollEvent m = {.xoff = (float)xoff, .yoff = (float)yoff};
    e.data.mouse_scroll = LinceNewCopy(&m, sizeof(LinceMouseScrollEvent));
    return e;
}