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

/* Helper functions to initialise and allocate events */
LinceEvent LinceNewKeyPressedEvent(int key, int repeats);
LinceEvent LinceNewKeyReleasedEvent(int key);
LinceEvent LinceNewKeyTypeEvent(int key);

#endif // LINCE_KEY_EVENT_H