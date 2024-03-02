
#include "event/window_event.h"
#include "utils/memory.h"


LinceEvent LinceNewWindowCloseEvent(){
    LinceEvent e = {
        .type = LinceEventType_WindowClose,
        .name = "LinceWindowCloseEvent",
        .handled = 0,
        .data = {0} // this event doesn't need to hold any data.
    };
    return e;
}

LinceEvent LinceNewWindowResizeEvent(int width, int height){
    LinceEvent e = {
        .type = LinceEventType_WindowResize,
        .name = "LinceWindowResizeEvent",
        .handled = 0,
        .data = {0}
    };
    LinceWindowResizeEvent w = {
        .width = (unsigned int)width,
        .height = (unsigned int)height
    };
    e.data.window_resize = LinceNewCopy(&w, sizeof(LinceWindowResizeEvent));
    return e;
}