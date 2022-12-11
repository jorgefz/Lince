#ifndef MCOMMAND_COLLIDER_H
#define MCOMMAND_COLLIDER_H


#include <lince.h>

typedef struct Collider {
	float x, y, vx, vy, w, h, angle;
} Collider;

// Check if two non-rotated rectangles overlap
LinceBool CollidersOverlap(Collider* a, Collider* b);

typedef struct GameObject{
	int type;

} GameObject;



#endif /* MCOMMAND_COLLIDER_H */