#ifndef LINCE_MOUSE_EVENT_H
#define LINCE_MOUSE_EVENT_H

#include "lince/event/event.h"

/** @struct LinceMouseButtonReleasedEvent
* @brief Propagated when mouse button is pressed.
* If a mouse button is held down, the event is propagated repeatedly.
*/
typedef struct LinceMouseButtonPressedEvent {
    int button; ///< Mouse button code. See `LinceMouseButton`.
} LinceMouseButtonPressedEvent;

/** @struct LinceMouseButtonReleasedEvent
* @brief Propagated when mouse button is released.
* More useful for game click events than mouse press events.
*/
typedef struct LinceMouseButtonReleasedEvent {
    int button; ///< Mouse button code. See `LinceMouseButton`.
} LinceMouseButtonReleasedEvent;

/** @struct LinceMouseMovedEvent
* @brief Propagated when the mouse cursor is moved.
*/
typedef struct LinceMouseMovedEvent {
    float xpos; ///< New X coordinate of the mouse on the screen
    float ypos; ///< New Y coordinate of the mouse on the screen
} LinceMouseMovedEvent;

/** @struct LinceMouseScrolledEvent
* @brief Propagated when the mouse wheel is scrolled
* or a touchpad gesture is made.
*/
typedef struct LinceMouseScrolledEvent {
    float xoff; ///< New X scroll offset. X-axis scrolling possible with a touchpad.
    float yoff; ///< New Y scroll offset. Specific values vary between machines. Typically, negative is down, and positive is up.
} LinceMouseScrolledEvent;

/** @brief Initialises a mouse button press event */
LinceEvent LinceNewMouseButtonPressedEvent(int button);

/** @brief Initialises a mouse button release event */
LinceEvent LinceNewMouseButtonReleasedEvent(int button);

/** @brief Initialises a mouse move event */
LinceEvent LinceNewMouseMovedEvent(double xpos, double ypos);

/** @brief Initialises a mouse scroll event */
LinceEvent LinceNewMouseScrolledEvent(double xoff, double yoff);


#endif // LINCE_MOUSE_EVENT_H