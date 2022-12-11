#include "timer.h"

LinceBool UpdateTimer(Timer* timer, float dt){
	// Sign of tick time tells if increasing or decreasing
	if( (timer->tick > 0.0f && timer->counter >= timer->end) ||
		(timer->tick < 0.0f && timer->counter <= timer->end)
	){
		timer->finished = LinceTrue;
	} else {
		timer->counter += timer->tick * dt;
	}
	return timer->finished;
}

void ResetTimer(Timer* timer){
	timer->counter = timer->start;
	timer->finished = LinceFalse;
}
