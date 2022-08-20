
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

#ifndef PONG_H
#define PONG_H

#include "lince.h"
#include "lince/gui/nuklear_flags.h"
#include "lince/gui/nuklear.h"

/* Layer */
LinceLayer* PongLayerInit();
void PongLayerOnAttach(LinceLayer* layer);
void PongLayerOnUpdate(LinceLayer* layer, float dt);
void PongLayerOnEvent (LinceLayer* layer, LinceEvent* event);
void PongLayerOnDetach(LinceLayer* layer);

#endif /* PONG_H */