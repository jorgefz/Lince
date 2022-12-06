
#include "event/key_event.h"

LinceEvent LinceNewKeyPressedEvent(int key, int repeats){
    LinceEvent e = {
        .type = LinceEventType_KeyPressed,
        .name = "LinceKeyPressedEvent",
        .handled = LinceFalse,
        .data = {0}
    };
    LinceKeyPressedEvent k = {
        .keycode = key,
        .repeats = repeats
    };
    e.data.KeyPressed = malloc(sizeof(LinceKeyPressedEvent));
    LINCE_ASSERT(e.data.KeyPressed, "Failed to allocate memory");
    *(e.data.KeyPressed) = k;
    return e;
}


LinceEvent LinceNewKeyReleasedEvent(int key){
    LinceEvent e = {
        .type = LinceEventType_KeyReleased,
        .name = "LinceKeyReleasedEvent",
        .handled = LinceFalse,
        .data = {0}
    };
    LinceKeyReleasedEvent k = {.keycode = key};
    e.data.KeyReleased = malloc(sizeof(LinceKeyReleasedEvent));
    LINCE_ASSERT(e.data.KeyReleased, "Failed to allocate memory");
    *(e.data.KeyReleased) = k;
    return e;
}


LinceEvent LinceNewKeyTypeEvent(int key){
    LinceEvent e = {
        .type = LinceEventType_KeyType,
        .name = "LinceKeyTypeEvent",
        .handled = LinceFalse,
        .data = {0}
    };
    LinceKeyTypeEvent k = {.keycode = key};
    e.data.KeyType = malloc(sizeof(LinceKeyTypeEvent));
    LINCE_ASSERT(e.data.KeyType, "Failed to allocate memory");
    *(e.data.KeyType) = k;
    return e;
}