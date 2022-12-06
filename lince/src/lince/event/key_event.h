#ifndef LINCE_KEY_EVENT_H
#define LINCE_KEY_EVENT_H

#include "lince/event/event.h"

typedef struct LinceKeyPressedEvent {
    int keycode, repeats;
} LinceKeyPressedEvent;

typedef struct LinceKeyReleasedEvent {
    int keycode;
} LinceKeyReleasedEvent;

typedef struct LinceKeyTypeEvent {
    int keycode;
} LinceKeyTypeEvent;

/* Helper functions to initialise and allocate events */
LinceEvent LinceNewKeyPressedEvent(int key, int repeats);
LinceEvent LinceNewKeyReleasedEvent(int key);
LinceEvent LinceNewKeyTypeEvent(int key);

#endif // LINCE_KEY_EVENT_H