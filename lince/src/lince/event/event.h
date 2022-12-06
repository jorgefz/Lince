#ifndef LINCE_EVENT_H
#define LINCE_EVENT_H

#include "lince/core/core.h"

typedef enum LinceEventType {
    LinceEventType_None = 0,
    LinceEventType_WindowClose, LinceEventType_WindowResize,
    LinceEventType_KeyPressed, LinceEventType_KeyReleased, LinceEventType_KeyType,
    LinceEventType_MouseButtonPressed, LinceEventType_MouseButtonReleased,
    LinceEventType_MouseMoved, LinceEventType_MouseScrolled,
    LinceEventType_EventNum // number of pre-defined events
} LinceEventType;

/* Handy way of accessing specific event data without casting */
typedef union LinceEventData {
    struct LinceKeyPressedEvent*          KeyPressed;
    struct LinceKeyReleasedEvent*         KeyReleased;
    struct LinceKeyTypeEvent*             KeyType;
    struct LinceWindowCloseEvent*         WindowClose;
    struct LinceWindowResizeEvent*        WindowResize;
    struct LinceMouseButtonPressedEvent*  MouseButtonPressed;
    struct LinceMouseButtonReleasedEvent* MouseButtonReleased;
    struct LinceMouseMovedEvent*          MouseMoved;
    struct LinceMouseScrolledEvent*       MouseScrolled;
    void*                                 GenericEvent;
} LinceEventData;

/* Stores event info that is propagated through the program */
typedef struct LinceEvent {
    LinceEventType type;
    char name[LINCE_NAME_MAX];
    LinceBool handled;
    LinceEventData data;
} LinceEvent;

/* Signature of function called when event is dispatched */
typedef LinceBool (*LinceEventFn)(LinceEvent*);

/* Calls given function to deal with event */
LinceBool LinceDispatchEvent(LinceEvent* e, LinceEventType etype, LinceEventFn func);

/* Deallocates and terminates an event */
void LinceEndEvent(LinceEvent* e);

#endif // LINCE_EVENT_H