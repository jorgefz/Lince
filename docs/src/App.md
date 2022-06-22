# App


## LinceApp
Structure members of `LinceApp`.

The global state of the application is stored in a `LinceApp` structure allocated on the stack. Prior to running the application, the user may change the program's settings via this structure.

Once the settings have been tuned, call the function `LinceRun()` to initialise the application.

### User settings
- `void* user_data`
	- Pointer for custom data set by the user

### User callbacks
These callbacks should be set before the applciation starts running.
- `void game_init(void)`
	- Function called when the program starts.
- `void game_on_update(float dt)`
	- Function called on every game tick.
	- The sole argument `float dt` is the delta time between frames.
- `void game_on_event(LinceEvent* event)`
	- Function called when engine event takes place
	- The argument `event` is the engine event that the user can handle.
- `void game_terminate(void)`
	- Function called when the application is closed.
	- You should free your allocated memory here.

### Internal state
These structure members handle the internal state of the application and do not act as user-defined settings.

- `LinceWindow* window`
	- Stores the state of the window as well as the handle to the GLFw window object.
- `LinceLayerStack* layer_stack`
	- Stack of rendering layers that are drawn from last to first.
	- These are added by the user.
- `LinceLayerStack* overlay_stack`
	- Stack of overlays, which are layers that are rendered on top of the game proper. 
	- These usually constitute UI elements and menus.
- `LinceBool running`
	- Boolean that indicates whether the program is running.
- `float time_ms`
	- Number of milliseconds since the start of the application.
- `float dt`
	- Time step in seconds between frames, calculated from the system clock.
- `int current_layer`
	- Index of the layer that is being rendered on the layer stack.
	- If no layer is active, this takes the value `-1`.
- `int current_overlay`
	- Index of the overlay that is being rendered on the overlay stack.
	- If no overlay is active, this takes the value `-1`.

## LinceGetAppState
```c
LinceApp* LinceGetAppState()
```

Returns a pointer the the application state and user settings.

Example:
```c
void Init() {
	printf(" Game started!\n");
}

void OnUpdate(float dt) {
	printf(" dt = %.3f ms\n", dt * 1000.0f);
}

void Terminate() {
	printf(" Game terminated!\n");
}

int main() {

	// setup callbacks
	LinceApp* app = LinceGetAppState();
	app->game_init = Init;
	app->game_on_update = OnUpdate;
	app->game_terminate = Terminate;

	// start the application
	LinceRun();

	return 0;
}
```


## LinceRun
```c
void LinceRun()
```
Creates a window and initialises the application. At this point, you should interact with the app via the callback functions.

## LinceCheckErrors
```c
void LinceCheckErrors()
```
Fetches OpenGL errors and stops the application if one is found.

## LinceGetTimeMillis
```c
double LinceGetTimeMillis()
```
Returns the number of milliseconds since the application started.

## LinceGetAspectRatio
```c
float LinceGetAspectRatio()
```
Returns the aspect ratio of the application window.