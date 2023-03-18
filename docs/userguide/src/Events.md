# Events

Events are generated when a certain action is executed, either by the player or by the application. Examples are mouse button clicks, key strokes, mouse movement, and window resizing. Every time an event takes place, a callback function is called and a `LinceEvent` object is generated and propagated through the engine. If set, your game will receive this events through a custom `OnEvent` function via the `game_on_event` callback (see [App](./App.md)).

Events are first passed to the application's `game_on_event` (see [App](./App.md)) and then to the OnEvent callback of each of the overlays and layers (see [Layers](./Layers.md)).

## LinceEvent
Structure that holds the data relevant to an event.

- `LinceEventType type`
	- Enum that describes the type of event (see [LinceEventType](##LinceEventType)). 
- `LinceBool handled`
	- Boolean that indicates whether the event should be propagated further.
- `LinceEventData data`
	- Union that holds the data for all the possible types of events
	- You should only retrieve the data corresponding to the event's type.
- `char name[LINCE_NAME_MAX]`
	- Debug name, unused.


## LinceEventType

Event types predefined by the engine.

| Enum  							 | Value |
| ---------------------------------- | ----- |
| LinceEventType_None				 |   0   | 
| LinceEventType_WindowClose		 |   1   | 
| LinceEventType_WindowResize		 |   2   | 
| LinceEventType_KeyPressed			 |   3   | 
| LinceEventType_KeyReleased		 |   4   | 
| LinceEventType_KeyType			 |   5   | 
| LinceEventType_MouseButtonPressed	 |   6   | 
| LinceEventType_MouseButtonReleased |   7   | 
| LinceEventType_MouseMoved			 |   8   | 
| LinceEventType_MouseScrolled		 |   9   | 
| LinceEventType_EventNum			 |   10  | 


## LinceDispatchEvent
```c
LinceBool LinceDispatchEvent(LinceEvent* event, LinceEventType event_type, LinceEventFn callback)
```

Handles a given event with a callback only if it matches the given event type.

Parameters:
- `LinceEvent* event`
	- Event to dispatch
- `LinceEventType event_type`
	- Desired type of the event to dispatch
- `LinceEventFn callback`: function that is called to handle the event if it is of the desired type. It must have the signature `LinceBool callback(LinceEvent*)` and should return `LinceTrue` (= 1) if it handled the event, and `LinceFalse` (= 0) otherwise.

Returns:
* `LinceBool`: boolean indicates whether event has been handled and should not be propagated further.

Example:
```c
LinceBool OnMouseClick(LinceEvent* e){
	printf("Mouse button clicked!\n");
	return LinceFalse;
}

void GameOnEvent(LinceEvent* e){
	LinceDispatchEvent(e, LinceEventType_MouseButtonPressed, OnMouseClick);
}
```


## KeyPressedEvent
Takes place when a key on the keyboard is pressed. Usually, this occurs once when the key is pressed, and then after a cooldown, it occurs repeatedly if the key is held down. The specific event data is retrieved via `LinceEvent.data.KeyPressed`.

Data:
- `int keycode`
	- Keycode as defined by the enum `LinceKey` (see [Input](./Input.md) chapter).
- `int repeats`
	- Number of repeat events generated when a key is held down. Currently unused.

Example:
```c
LinceBool OnKeyPress(LinceEvent* e){
	KeyPressedEvent* kp = e->data.KeyPressed;
	printf(" Key pressed: %d\n", kp->keycode);
	return LinceFalse;
}
```

## KeyReleasedEvent
Takes place when a keyboard key is released. The specific event data is retrieved via `LinceEvent.data.KeyReleased`.

Data:
- `int keycode`
	- Key code as defined by the enum `LinceKey` (see [Input](./Input.md) chapter).

Example:
```c
LinceBool OnKeyRelease(LinceEvent* e){
	KeyReleasedEvent* kr = e->data.KeyReleased;
	printf(" Key released: %d\n", kr->keycode);
	return LinceFalse;
}
```

## KeyTypeEvent
Takes place when text is being typed. The specific event data is retrieved via `LinceEvent.data.KeyType`.

Data:
- `int keycode`
	- Key code as defined by the enum `LinceKey` (see [Input](./Input.md) chapter).

Example:
```c
LinceBool OnKeyType(LinceEvent* e){
	KeyTypeEvent* kt = e->data.KeyType;
	printf("Key typed: %d\n", kt->keycode);
	return LinceFalse;
}
```

## WindowCloseEvent
Takes place when the window has been signaled to close. The specific event data is retrieved via `LinceEvent.data.WindowClose`, although it holds no usable data but a dummy heap-allocated integer whose value is unused.

Data:
- `int dummy`
	- Unused placeholder data. The event being generated in the first place is indicative that the window should close.

Example:
```c
LinceBool OnWindowClose(LinceEvent* e){
	printf(" Closing window!\n");
	FreeMemory();
	TerminateGame();
	return LinceTrue;
}
```

## WindowResizeEvent
Takes place when the size or shape of the window is changed. The specific event data is retrieved via `LinceEvent.data.WindowResize`.

Data:
- `unsigned int height`
	- New height of the window in pixels.
- `unsigned int width`
	- New width of the window in pixels.

Example:
```c
LinceBool OnWindowResize(LinceEvent* e){
	WindowResizeEvent* wr = e->data.WindowResize;
	printf(" Window resized to %ux%u\n", wr->width, wr->height);
	return LinceFalse;
}
```

## MouseButtonPressedEvent
Takes place when a mouse button is clicked. The specific event data is retrieved via `LinceEvent.data.MouseButtonPressed`.

Data:
- `int button`
	- Button code defined by the enum `LinceMouseButton` (see [Input](./Input.md) chapter).

Example:
```c
LinceBool OnMouseButtonPress(LinceEvent* e){
	MouseButtonPressedEvent* mbp = e->data.MouseButtonPressed;
	printf(" Mouse button pressed: %d\n", mbp->button);
	return LinceFalse;
}
```

## MouseButtonReleasedEvent
Takes place when a mouse button is released. The specific event data is retrieved via `LinceEvent.data.MouseButtonReleased`.

Data:
- `int button`
	- Button code defined by the enum `LinceMouseButton` (see [Input](./Input.md) chapter).

Example:
```c
LinceBool OnMouseButtonRelease(LinceEvent* e){
	MouseButtonReleasedEvent* mbr = e->data.MouseButtonReleased;
	printf(" Mouse button released: %d\n", mbr->button);
	return LinceFalse;
}
```

## MouseMovedEvent
Takes place when the mouse is moved anywhere on the screen. The specific event data is retrieved via `LinceEvent.data.MouseMoved`.

Data:
- `float xpos`
	- New X position of the mouse in screen coordinates with origin on the top-left corner of the window.
- `float ypos`
	- New Y position of the mouse in screen coordinates with origin on the top-left corner of the window.

Example:
```c
LinceBool OnMouseMoved(LinceEvent* e){
	MouseMovedEvent* mm = e->data.MouseMoved;
	printf(" Mouse moved to %.3f,%.3f\n", mm->xpos, mm->ypos);
	return LinceFalse;
}
```

## MouseScrolledEvent
Takes place when scrolling occurs, either via the mouse wheel or a touchpad gesture. Typical mouse wheel scrolling only provides a Y coordinate, whereas touchpad scrolling is two dimensional (zooming, panning, etc). The specific event data is retrieved via `LinceEvent.data.MouseScrolled`.

Data:
- `float xoff`
	- Magnitude of the scroll on the X axis
- `float yoff`
	- Magnitude of the scroll on the Y axis

Example:
```c
LinceBool OnMouseScrolled(LinceEvent* e){
	MouseScrolledEvent* mm = e->data.MouseScrolled;
	printf(" Mouse scrolled by %.3f,%.3f\n", mm->xoff, mm->yoff);
	return LinceFalse;
}
```

## GenericEvent
This is an additional member of the `LinceEvent.data` union that can point to any raw data via a `void*` pointer.
