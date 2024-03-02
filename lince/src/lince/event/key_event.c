
#include "event/key_event.h"
#include "utils/memory.h"

LinceEvent LinceNewKeyPressEvent(int key, int repeats){
    LinceEvent e = {
        .type = LinceEventType_KeyPress,
        .name = "LinceKeyPressEvent",
        .handled = LinceFalse,
        .data = {0}
    };
    LinceKeyPressEvent k = {
        .keycode = key,
        .repeats = repeats
    };
    e.data.key_press = LinceNewCopy(&k, sizeof(LinceKeyPressEvent));
    return e;
}


LinceEvent LinceNewKeyReleaseEvent(int key){
    LinceEvent e = {
        .type = LinceEventType_KeyRelease,
        .name = "LinceKeyReleaseEvent",
        .handled = LinceFalse,
        .data = {0}
    };
    LinceKeyReleaseEvent k = {.keycode = key};
    e.data.key_release = LinceNewCopy(&k, sizeof(LinceKeyReleaseEvent));
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
    e.data.key_type = LinceNewCopy(&k, sizeof(LinceKeyTypeEvent));
    return e;
}