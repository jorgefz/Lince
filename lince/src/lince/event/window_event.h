#ifndef LINCE_WINDOW_EVENT_H
#define LINCE_WINDOW_EVENT_H

#include "lince/event/event.h"

/** @struct LinceWindowCloseEvent
* @brief Signals that the application should terminate.
*/
typedef struct LinceWindowCloseEvent {
    int dummy; ///< (unused)
} LinceWindowCloseEvent;

/** @struct LinceWindowResizeEvent
* @brief Propagated when the window changes is resized.
* Useful to adapt the view-projection matrix for changes in the aspect ratio.
*/
typedef struct LinceWindowResizeEvent {
    uint32_t height; ///< The new height of the windows
    uint32_t width;  ///< The new width of the windows
} LinceWindowResizeEvent;

/** @brief Initialises a window close */
LinceEvent LinceNewWindowCloseEvent();

/** @brief Initialises a window resize */
LinceEvent LinceNewWindowResizeEvent(int width, int height);

#endif // LINCE_WINDOW_EVENT_H