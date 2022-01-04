
#include "event/windowEvent.h"


LinceEvent LinceEvent_NewWindowCloseEvent(){
    LinceEvent e = {
        .type = LinceEventType_WindowClose,
        .name = "WindowCloseEvent",
        .handled = 0,
        .data = {0} // this event doesn't need to hold any data.
    };
    return e;
}

LinceEvent LinceEvent_NewWindowResizeEvent(int width, int height){
    LinceEvent e = {
        .type = LinceEventType_WindowResize,
        .name = "WindowResizeEvent",
        .handled = 0,
        .data = {0}
    };
    WindowResizeEvent w = {
        .width = (unsigned int)width,
        .height = (unsigned int)height
    };
    e.data.WindowResize = malloc(sizeof(WindowResizeEvent));
    LINCE_ASSERT(e.data.WindowResize, "Failed to allocate memory\n");
    *(e.data.WindowResize) = w;
    return e;
}