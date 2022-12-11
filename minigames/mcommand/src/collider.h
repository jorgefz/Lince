#ifndef MCOMMAND_COLLIDER_H
#define MCOMMAND_COLLIDER_H


#include <lince.h>

typedef struct Collider {
	float x, y, vx, vy, w, h, angle;
} Collider;

// Check if two non-rotated rectangles overlap
LinceBool CollidersOverlap(Collider* a, Collider* b);


typedef struct GameObject{
	Collider* collider_comp;
	LinceQuadProps* sprite_comp;
	// Timer* timer_comp;
} GameObject;



#endif /* MCOMMAND_COLLIDER_H */