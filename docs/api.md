
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
void Lince_SetGameInitFnvoid( void (*init)() )
```
Sets the game initialization function, called once when Lince is initialised, before the main rendering loop.

```c
void Lince_SetGameOnUpdateFnvoid( void (*on_update)() )
```
Sets the game update function, called once every frame.

```c
void Lince_SetGameOnEventFn( void (*on_event)(Event*) )
```
Sets the game event function, called whenever a game event takes place (e.g. mouse click)

```c
void Lince_SetGameTerminateFn( void (*terminate)() )
```
Sets the function to be called when the rendering loop is terminated.


## Layers


## Events