#ifndef LINCE_KEY_EVENT_H
#define LINCE_KEY_EVENT_H

#include "event/event.h"

typedef struct KeyPressedEvent {
    int keycode, repeats;
} KeyPressedEvent;

typedef struct KeyReleasedEvent {
    int keycode;
} KeyReleasedEvent;

typedef struct KeyTypeEvent {
    int keycode;
} KeyTypeEvent;

Event LinceEvent_NewKeyPressedEvent(int key, int repeats);
Event LinceEvent_NewKeyReleasedEvent(int key);
Event LinceEvent_NewKeyTypeEvent(int key);

#endif // LINCE_KEY_EVENT_H