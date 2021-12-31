
#include "event/windowEvent.h"


Event LinceEvent_NewWindowCloseEvent(){
    Event e = (Event){
        .type = EventType_WindowClose,
        .name = "WindowCloseEvent",
        .handled = 0,
        .data = {0} // this event doesn't need to hold any data.
    };
    return e;
}

Event LinceEvent_NewWindowResizeEvent(int width, int height){
    Event e = (Event){
        .type = EventType_WindowResize,
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