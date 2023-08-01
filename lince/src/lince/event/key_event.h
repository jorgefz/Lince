#ifndef LINCE_KEY_EVENT_H
#define LINCE_KEY_EVENT_H

#include "lince/event/event.h"

/** @struct LinceKeyPressEvent
* @brief Takes place when a key on the keyboard is pressed.
* 
* This is propagated once when the key is pressed
* If the key is held down, after a cooldown,
* the event is propagated on every frame repeatedly.
* The specific event data is retrieved via `LinceEvent.data.key_press`.
*/
typedef struct LinceKeyPressEvent {
    int keycode; ///< Code of pressed key. See `LinceKey`.
    int repeats; ///< (unused)
} LinceKeyPressEvent;

/** @struct LinceKeyReleaseEvent
* @brief Propagated when a key is released.
*
* The specific event data is retrieved via `LinceEvent.data.key_release`.
*/
typedef struct LinceKeyReleaseEvent {
    int keycode; ///< Code of released key. See `LinceKey`. 
} LinceKeyReleaseEvent;

/** @struct LinceKeyTypeEvent
* @brief Propagated when a key is typed in a text field.
* Typing mode is enabled by certain APIs, such as Nuklear,
* to collect text input from the user.
*
* The specific event data is retrieved via `LinceEvent.data.key_type`.
*/
typedef struct LinceKeyTypeEvent {
    int keycode; ///< Code of typed key. See `LinceKey`.
} LinceKeyTypeEvent;

/** @brief Initialise a key press event */
LinceEvent LinceNewKeyPressEvent(int key, int repeats);

/** @brief Initialise a key release event */
LinceEvent LinceNewKeyReleaseEvent(int key);

/** @brief Initialise a key type event */
LinceEvent LinceNewKeyTypeEvent(int key);

#endif // LINCE_KEY_EVENT_H