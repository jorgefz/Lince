
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

/* Layer */
LinceLayer* GameLayerInit();
void GameLayerOnAttach(LinceLayer* layer);
void GameLayerOnUpdate(LinceLayer* layer, float dt);
void GameLayerOnEvent (LinceLayer* layer, LinceEvent* event);
void GameLayerOnDetach(LinceLayer* layer);

#endif /* PONG_H */