/** @file event.h
Events are generated when a certain action is executed,
either by the user or by the application.
Examples are mouse button clicks, key strokes, mouse movement, and window resizing.
Every time an event takes place, a callback function is called
and a `LinceEvent` object is generated and propagated through the engine.
If set, your game will receive this events through a custom `OnEvent` function
via the `game_on_event` callback.

Events are first passed to the application's `game_on_event`
and then to the OnEvent callbacks of each of the overlays and layers.
*/

#ifndef LINCE_EVENT_H
#define LINCE_EVENT_H

#include "lince/core/core.h"

/** @enum LinceEventType
* @brief Describes the trigger for the event, e.g. key press, or window resize.
*/
typedef enum LinceEventType {
    LinceEventType_None = 0,
    LinceEventType_WindowClose, LinceEventType_WindowResize,
    LinceEventType_KeyPressed, LinceEventType_KeyReleased, LinceEventType_KeyType,
    LinceEventType_MouseButtonPressed, LinceEventType_MouseButtonReleased,
    LinceEventType_MouseMoved, LinceEventType_MouseScrolled,
    LinceEventType_EventNum ///< number of pre-defined events
} LinceEventType;

/** @union LinceEventData
* @brief Handy way of accessing specific event data without casting
*/
typedef union LinceEventData {
    struct LinceKeyPressedEvent*          KeyPressed;
    struct LinceKeyReleasedEvent*         KeyReleased;
    struct LinceKeyTypeEvent*             KeyType;
    struct LinceWindowCloseEvent*         WindowClose;
    struct LinceWindowResizeEvent*        WindowResize;
    struct LinceMouseButtonPressedEvent*  MouseButtonPressed;
    struct LinceMouseButtonReleasedEvent* MouseButtonReleased;
    struct LinceMouseMovedEvent*          MouseMoved;
    struct LinceMouseScrolledEvent*       MouseScrolled;
    void*                                 GenericEvent;
} LinceEventData;

/** @struct LinceEvent
* @brief Stores event info that is propagated through the program
*/
typedef struct LinceEvent {
    LinceEventType type;        ///< Trigger
    char name[LINCE_NAME_MAX];  ///< Event string representation
    LinceBool handled;          ///< Set to tru if program has responded and you don't want to propagate it further.
    LinceEventData data;        ///< Specific data to the event, e.g. key code.
} LinceEvent;

/** @typedef LinceEventFn
* @brief Signature of function called when event is dispatched
*/
typedef LinceBool (*LinceEventFn)(LinceEvent*);

/** @brief Calls given function to deal with event
* @param event Event to respond to
* @param type Event type you want to respond to.
* @param func Callback called if event type matches input type.
*/
LinceBool LinceDispatchEvent(LinceEvent* event, LinceEventType type, LinceEventFn func);

/** @brief Deallocates and terminates an event */
void LinceEndEvent(LinceEvent* e);

#endif // LINCE_EVENT_H