
/*
	===== PONG =====

	Simple game of Pong built with the Lince engine.

	Player BLUE (left)
		W - paddle up
		S - paddle down

	Player RED (right)
		UP   - paddle up
		DOWN - paddle down

	P     - pause game
	SPACE - resume game

*/



#include "lince.h"
#include "gui/nuklear_flags.h"
#include "gui/nuklear.h"

typedef struct PongObject {
	float w, h;   // size
	float x, y;   // position
	float vx, vy; // speed
} PongObject;


/* Layer */
LinceLayer* PongLayerInit();
void PongLayerOnAttach(LinceLayer* layer);
void PongLayerOnUpdate(LinceLayer* layer, float dt);
void PongLayerOnEvent (LinceLayer* layer, LinceEvent* event);
void PongLayerOnDetach(LinceLayer* layer);