#ifndef LINCE_EVENT_H
#define LINCE_EVENT_H

#include "core.h"

typedef enum {
    EventType_None = 0,
    EventType_WindowClose, EventType_WindowResize,
    EventType_KeyPressed, EventType_KeyReleased, EventType_KeyType,
    EventType_MouseButtonPressed, EventType_MouseButtonReleased,
    EventType_MouseMoved, EventType_MouseScrolled,
    // add new event types here
    EventType_EventNum
} EventType;

typedef union EventData {
    struct KeyPressedEvent* KeyPressed;
    struct KeyReleasedEvent* KeyReleased;
    struct KeyTypeEvent* KeyType;
    struct WindowCloseEvent* WindowClose;
    struct WindowResizeEvent* WindowResize;
    struct MouseButtonPressedEvent* MouseButtonPressed;
    struct MouseButtonReleasedEvent* MouseButtonReleased;
    struct MouseMovedEvent* MouseMoved;
    struct MouseScrolledEvent* MouseScrolled;
    // add new event declarations here
    void* GenericEvent;
} EventData;

typedef struct Event {
    EventType type;
    char name[LINCE_STR_MAX];
    lince_bool handled;
    EventData data;
} Event;

typedef unsigned int (*EventFn)(Event*);

// Functions
unsigned int LinceEvent_Dispatch(Event* e, EventType etype, EventFn func);
void LinceEvent_Destroy(Event* e);

#endif // LINCE_EVENT_H