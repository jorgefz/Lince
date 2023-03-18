# Lince Game Engine

Lince is a game engine written entirely in C targetted towards 2D games. The engine is currently a work in progress, and it's so early in its development that it doesn't even qualify as pre-alpha - the basic foundations of the renderer are being built and it won't be usable for some time.

## Basic Code Example

```c
#include <stdio.h>
#include "lince.h"

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

	LinceApp* app = LinceGetAppState();

	app->game_init = Init;
	app->game_on_update = OnUpdate;
	app->game_terminate = Terminate;

	LinceRun();

	return 0;
}
```