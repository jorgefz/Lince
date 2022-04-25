
#include "event/key_event.h"

LinceEvent LinceNewKeyPressedEvent(int key, int repeats){
    LinceEvent e = {
        .type = LinceEventType_KeyPressed,
        .name = "KeyPressedEvent",
        .handled = LinceFalse,
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


LinceEvent LinceNewKeyReleasedEvent(int key){
    LinceEvent e = {
        .type = LinceEventType_KeyReleased,
        .name = "KeyReleasedEvent",
        .handled = LinceFalse,
        .data = {0}
    };
    KeyReleasedEvent k = {.keycode = key};
    e.data.KeyReleased = malloc(sizeof(KeyReleasedEvent));
    LINCE_ASSERT(e.data.KeyReleased, "Failed to allocate memory");
    *(e.data.KeyReleased) = k;
    return e;
}


LinceEvent LinceNewKeyTypeEvent(int key){
    LinceEvent e = {
        .type = LinceEventType_KeyType,
        .name = "KeyTypeEvent",
        .handled = LinceFalse,
        .data = {0}
    };
    KeyTypeEvent k = {.keycode = key};
    e.data.KeyType = malloc(sizeof(KeyTypeEvent));
    LINCE_ASSERT(e.data.KeyType, "Failed to allocate memory");
    *(e.data.KeyType) = k;
    return e;
}