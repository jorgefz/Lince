#ifndef MCOMMAND_TIMER_H
#define MCOMMAND_TIMER_H

#include <lince.h>

typedef struct Timer{
	float start, end; 	// starting and final values
	float tick;			// added to the counter each time UpdateTimer is called
	float counter;		// keeps track of current value
	LinceBool finished; // Set to true if counter has reached end, and false otherwise
} Timer;

LinceBool UpdateTimer(Timer* timer, float dt);

void ResetTimer(Timer* timer);

#endif /* MCOMMAND_TIMER_H */