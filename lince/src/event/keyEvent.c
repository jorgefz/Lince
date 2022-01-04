
#include "event/keyEvent.h"

LinceEvent LinceEvent_NewKeyPressedEvent(int key, int repeats){
    LinceEvent e = {
        .type = LinceEventType_KeyPressed,
        .name = "KeyPressedEvent",
        .handled = 0,
        .data = {0}
    };
    KeyPressedEvent k = {
        .keycode = key,
        .repeats = repeats
    };
    e.data.KeyPressed = malloc(sizeof(KeyPressedEvent));
    LINCE_ASSERT(e.data.KeyPressed, "Failed to allocate memory");
    *(e.data.KeyPressed) = k;
    return e;
}


LinceEvent LinceEvent_NewKeyReleasedEvent(int key){
    LinceEvent e = {
        .type = LinceEventType_KeyReleased,
        .name = "KeyReleasedEvent",
        .handled = 0,
        .data = {0}
    };
    KeyReleasedEvent k = {.keycode = key};
    e.data.KeyReleased = malloc(sizeof(KeyReleasedEvent));
    LINCE_ASSERT(e.data.KeyReleased, "Failed to allocate memory");
    *(e.data.KeyReleased) = k;
    return e;
}


LinceEvent LinceEvent_NewKeyTypeEvent(int key){
    LinceEvent e = {
        .type = LinceEventType_KeyType,
        .name = "KeyTypeEvent",
        .handled = 0,
        .data = {0}
    };
    KeyTypeEvent k = {.keycode = key};
    e.data.KeyType = malloc(sizeof(KeyTypeEvent));
    LINCE_ASSERT(e.data.KeyType, "Failed to allocate memory");
    *(e.data.KeyType) = k;
    return e;
}