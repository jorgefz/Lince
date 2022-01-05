
# Lince

An OpenG game engine written in C


## Code example
```c
#include <stdio.h>
#include "lince.h"

void MyGameInit(){
	// runs on startup
	printf("Game started!\n");
}

void MyGameOnUpdate(){
	// runs every frame
}

void MyGameOnEvent(Event* e){
	// runs when event is called
}

void MyGameTerminate(){
	// runs when game is closing
	printf("Game is closing!\n");
}

int main(){

	Lince_SetGameInitFn(MyGameInit);
    Lince_SetGameOnUpdateFn(MyGameOnUpdate);
    Lince_SetGameOnEventFn(MyGameOnEvent);
    Lince_SetGameTerminateFn(MyGameTerminate);

	LinceApp_Run();

	return 0;
}

```


## Entry Point

```c
void LinceApp_Run()
```
Initialises the game engine and runs the main rendering loop. This function takes no arguments and returns no values, and should only be called once.


## Interacting with Lince

In order for the engine to communicate with your application and vice-versa, you should provide the engine with a set of functions

These should be set before `LinceApp_Run` is called.

```c
void Lince_SetGameInitFn( void (*init)() )
```
Sets the game initialization function, called once when Lince is initialised, before the main rendering loop.

```c
void Lince_SetGameOnUpdateFn( void (*on_update)() )
```
Sets the game update function, called once every frame.

```c
void Lince_SetGameOnEventFn( void (*on_event)(LinceEvent*) )
```
Sets the game event function, called whenever a game event takes place (e.g. mouse click)

```c
void Lince_SetGameTerminateFn( void (*terminate)() )
```
Sets the function to be called when the rendering loop is terminated.


## Events
Events are generated when a certain action is executed, either by the player or by the application. Examples are mouse button clicks, key strokes, mouse movement, and window resizing. Every time an event takes place, a callback function is called and a `LinceEvent` object is generated and propagated through the engine. If set, your game will receive this events through a custom `OnEvent` function, set with `Lince_SetGameOnEventFn(func)`. The input function should have a signature compatible with one input argument, `LinceEvent*`, and returning nothing (`void`).

```c
void GameOnEvent(LinceEvent* e);
```

Events can be dispatched (dealt with) by using the function `int LinceEvent_Dispatch(LinceEvent*, LinceEventType, LinceEventFn)`, which takes an event, a desired event type, and a function pointer that will only be called if the input event matches the desired type.

The second argument is the event type. Events that are pre-defined in the engine will have a unique event type, obtained with the parameter `LinceEvent->type`, which will have one of the following enum values:

* LinceEventType_None = 0
* LinceEventType_KeyPressed		
* LinceEventType_KeyReleased
* LinceEventType_KeyType
* LinceEventType_WindowClose
* LinceEventType_WindowResize
* LinceEventType_MouseButtonPressed
* LinceEventType_MouseButtonReleased
* LinceEventType_MouseMoved
* LinceEventType_MouseScrolled
* LinceEventType_EventNum

The third argument is a function pointer of signature `int function (LinceEvent*)`, which takes an event of known type, performs an action, and returns whether it has been handled (`1` or `LinceTrue`) or not (`0` or `LinceFalse`). A handled event will not be further propagated through the application.

```c

LinceBool GameOnKeyPressed(LinceEvent* e){
	// only called if its a key press event
	// your code here
	return LinceFalse;
}

void GameOnEvent(LinceEvent* e){
	LinceEvent_Dispatch(e, LinceEventType_KeyPressed, GameOnKeyPressed);
}

```

```c

LinceBool GameOnKeyPressed(LinceEvent* e){
	int key = e->data.KeyPressed->keycode;
	printf("Key %d pressed\n", key);
	return LinceFalse;
}

void GameOnEvent(LinceEvent* e){
	LinceEvent_Dispatch(e, LinceEventType_KeyPressed, GameOnKeyPressed);
}

```


The unique data that corresponds to each event type can be accessed through the parameter `LinceEvent->data`, an `union` type that contains pointers to every pre-defined event as well as a generic `void* GenericEvent` for user-defined events. For instance, the code below prints out the keycode every time a key is pressed:

```c

LinceBool GameOnKeyPressed(LinceEvent* e){
	int key = e->data.KeyPressed->keycode;
	printf("Key %d pressed\n", key);
	return LinceFalse;
}

void GameOnEvent(LinceEvent* e){
	LinceEvent_Dispatch(e, LinceEventType_KeyPressed, GameOnKeyPressed);
}

```

### Key Press Event

Generated every time a keyboard button is pressed.

* Event type enum: `LinceEventType_KeyPressed`
* Event data: `LinceEvent->data.KeyPressed->`
	* `int keycode`: unique code of the key pressed, matching GLFW codes (see `keycodes.h`).
	* `int repeats`: for keys that are being held down, number of times the key press event has been generated.

### Key Release Event

Generated every time a keyboard button is released.

* Event type enum: `LinceEventType_KeyReleased`
* Event data: `LinceEvent->data.KeyReleased->`
	* `int keycode`: unique code of the key pressed, matching GLFW codes (see `keycodes.h`).

### Key Type Event

Generated every time a keyboard button is pressed during character input.

* Event type enum: `LinceEventType_KeyType`
* Event data: `LinceEvent->data.KeyType->`
	* `int keycode`: unique code of the key pressed, matching GLFW codes (see `keycodes.h`).

### Window Close Event

Generated when the window is being closed.

* Event type enum: `LinceEventType_WindowClose`
* Event data: `LinceEvent->data.WindowClose`
	* This event has no parameters

### Window Resize Event

Generated every time the window is resized.

* Event type enum: `LinceEventType_WindowResize`
* Event data: `LinceEvent->data.WindowResize->`
	* `float width`: new width of the window.
	* `float height`: new height of the window.

### Mouse Button Press Event

Generated every time a mouse button is pressed.

* Event type enum: `LinceEventType_MouseButtonPressed`
* Event data: `LinceEvent->data.MouseButtonPressed->`
	* `int button`: unique code of the button pressed, matching GLFW codes (see `mousecodes.h`).

### Mouse Button Release Event

Generated every time a mouse button is released.

* Event type enum: `LinceEventType_MouseButtonReleased`
* Event data: `LinceEvent->data.MouseButtonReleased->`
	* `int button`: unique code of the button released, matching GLFW codes (see `mousecodes.h`).

### Mouse Move Event

Generated every time the mouse is moved in the window.

* Event type enum: `LinceEventType_MouseMoved`
* Event data: `LinceEvent->data.MouseMoved->`
	* `float xpos`: horizontal position of the mouse on the window in pixels, with the origin on the left.
	* `float ypos`: vertical position of the mouse on the window in pixels, with the origin at the top.

### Mouse Scroll Event

Generated every time the mouse wheel or touchpad is scrolled.

* Event type enum: `LinceEventType_MouseScrolled`
* Event data: `LinceEvent->data.MouseScrolled->`
	* `float xoff`: horizontal position of the mouse on the window in pixels, with the origin on the left.
	* `float yoff`: vertical position of the mouse on the window in pixels, with the origin at the top.


## Layers

Layers are groups of geometry and shaders that are rendered together. Layers are rendered in order, from first to last. For instance, in a game, the background may be drawn first, then the map on top, the characters, and, finally, the UI. Overlays are layers that are typically drawn last, such as the UI.


### `LinceLayer`

Structure that represents a layer.

Data:
* `void* data.GenericLayer`: pointer to custom data for each type of layer.

Methods (function pointers):
* `void OnAttach(LinceLayer* layer)`: called when layer is added to application's layer stack.

* `void OnDetach(LinceLayer* layer)`: called when layer is removed from the application's layer stack (is destroyed).

* `void OnUpdate(LinceLayer* layer)`: called on every frame.

* `void OnEvent(LinceLayer* layer, LinceEvent* e)`: called when an unhandled event takes place.



### `LinceLayer* LinceLayer_Create(void* data)`

Creates and allocates a layer with custom data, but does not define its methods.

Arguments:
* `void* data`: custom data to store in the layer.

Returns:
* `LinceLayer*`: heap-allocated layer object.



## Lince Application

### `void LinceApp_PushLayer(LinceLayer* layer)`
Pushes input layer onto the layer stack.


### `void LinceApp_PushOverlay(LinceLayer* overlay)`
Pushes input layer onto the overlay stack.