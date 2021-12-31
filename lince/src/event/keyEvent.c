
#include "event/keyEvent.h"

Event LinceEvent_NewKeyPressedEvent(int key, int repeats){
    Event e = (Event){
        .type = EventType_KeyPressed,
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


Event LinceEvent_NewKeyReleasedEvent(int key){
    Event e = (Event){
        .type = EventType_KeyReleased,
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


Event LinceEvent_NewKeyTypeEvent(int key){
    Event e = (Event){
        .type = EventType_KeyType,
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