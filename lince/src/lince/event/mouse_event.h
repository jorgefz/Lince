#ifndef LINCE_MOUSE_EVENT_H
#define LINCE_MOUSE_EVENT_H

#include "lince/event/event.h"

/** @struct LinceMousePressEvent
* @brief Propagated when mouse button is pressed.
* If a mouse button is held down, the event is propagated repeatedly.
*/
typedef struct LinceMousePressEvent {
    int button; ///< Mouse button code. See `LinceMouseButton`.
} LinceMousePressEvent;

/** @struct LinceMouseReleaseEvent
* @brief Propagated when mouse button is released.
* More useful for game click events than mouse press events.
*/
typedef struct LinceMouseReleaseEvent {
    int button; ///< Mouse button code. See `LinceMouseButton`.
} LinceMouseReleaseEvent;

/** @struct LinceMouseMoveEvent
* @brief Propagated when the mouse cursor is moved.
*/
typedef struct LinceMouseMoveEvent {
    float xpos; ///< New X coordinate of the mouse on the screen
    float ypos; ///< New Y coordinate of the mouse on the screen
} LinceMouseMoveEvent;

/** @struct LinceMouseScrollEvent
* @brief Propagated when the mouse wheel is scrolled
* or a touchpad gesture is made.
*/
typedef struct LinceMouseScrollEvent {
    float xoff; ///< New X scroll offset. X-axis scrolling possible with a touchpad.
    float yoff; ///< New Y scroll offset. Specific values vary between machines. Typically, negative is down, and positive is up.
} LinceMouseScrollEvent;

/** @brief Initialises a mouse button press event */
LinceEvent LinceNewMousePressEvent(int button);

/** @brief Initialises a mouse button release event */
LinceEvent LinceNewMouseReleaseEvent(int button);

/** @brief Initialises a mouse move event */
LinceEvent LinceNewMouseMoveEvent(double xpos, double ypos);

/** @brief Initialises a mouse scroll event */
LinceEvent LinceNewMouseScrollEvent(double xoff, double yoff);


#endif // LINCE_MOUSE_EVENT_H