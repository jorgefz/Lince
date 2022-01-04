#ifndef LINCE_EVENT_H
#define LINCE_EVENT_H

#include "core.h"

typedef enum LinceEventType {
    LinceEventType_None = 0,
    LinceEventType_WindowClose, LinceEventType_WindowResize,
    LinceEventType_KeyPressed, LinceEventType_KeyReleased, LinceEventType_KeyType,
    LinceEventType_MouseButtonPressed, LinceEventType_MouseButtonReleased,
    LinceEventType_MouseMoved, LinceEventType_MouseScrolled,
    // add new event types here
    EventType_EventNum // number of pre-defined events
} LinceEventType;

typedef union LinceEventData {
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
} LinceEventData;

typedef struct LinceEvent {
    LinceEventType type;
    char name[LINCE_STR_MAX];
    LinceBool handled;
    LinceEventData data;
} LinceEvent;

typedef LinceBool (*LinceEventFn)(LinceEvent*);

// Functions
LinceBool LinceEvent_Dispatch(LinceEvent* e, LinceEventType etype, LinceEventFn func);
void LinceEvent_Destroy(LinceEvent* e);

#endif // LINCE_EVENT_H