
#include "event/key_event.h"
#include "utils/memory.h"

LinceEvent LinceNewKeyPressEvent(int key, int repeats, int mods){
    LinceEvent e = {
        .type = LinceEventType_KeyPress,
        .name = "LinceKeyPressEvent",
        .handled = LinceFalse,
        .data = {0}
    };
    LinceKeyPressEvent k = {
        .keycode = key,
        .repeats = repeats,
        .mods    = mods
    };
    e.data.key_press = LinceNewCopy(&k, sizeof(LinceKeyPressEvent));
    return e;
}


LinceEvent LinceNewKeyReleaseEvent(int key, int mods){
    LinceEvent e = {
        .type = LinceEventType_KeyRelease,
        .name = "LinceKeyReleaseEvent",
        .handled = LinceFalse,
        .data = {0}
    };
    LinceKeyReleaseEvent k = {.keycode = key, .mods = mods};
    e.data.key_release = LinceNewCopy(&k, sizeof(LinceKeyReleaseEvent));
    return e;
}


LinceEvent LinceNewKeyTypeEvent(uint32_t codepoint){
    // Received codepoints are Unicode UTF-32, which may be encoded to UTF-8.
    LinceEvent e = {
        .type = LinceEventType_KeyType,
        .name = "LinceKeyTypeEvent",
        .handled = LinceFalse,
        .data = {0}
    };
    LinceKeyTypeEvent k = {.keycode = codepoint};
    e.data.key_type = LinceNewCopy(&k, sizeof(LinceKeyTypeEvent));
    return e;
}