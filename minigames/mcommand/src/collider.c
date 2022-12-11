#include "collider.h"

// Check if two non-rotatde rectangles overlap
LinceBool CollidersOverlap(Collider* a, Collider* b){
	LinceBool overlap =
	a->x + a->w / 2.0f > b->x - b->w / 2.0f &&
    a->x - a->w / 2.0f < b->x + b->w / 2.0f &&
    a->y + a->h / 2.0f > b->y - b->h / 2.0f &&
    a->y - a->h / 2.0f < b->y + b->h / 2.0f;

	return overlap;
}
