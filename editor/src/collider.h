#ifndef LINCE_COLLIDER_H
#define LINCE_COLLIDER_H

#include "cglm/vec2.h"

typedef enum LinceColliderFlags{
    LinceCollision_None  = 0x0,
    LinceCollision_Right = 0x1,
    LinceCollision_Left  = 0x2,
    LinceCollision_Top   = 0x4,
    LinceCollision_Bot   = 0x8,
    LinceCollision_X = LinceCollision_Right | LinceCollision_Left,
    LinceCollision_Y = LinceCollision_Top   | LinceCollision_Bot,

} LinceColliderFlags;


typedef struct LinceCollider{
	vec2 pos;  // center x,y
    vec2 size; // width, height
} LinceCollider;


int LinceCollideBox(LinceCollider* a, LinceCollider* b);

#endif /* LINCE_COLLIDER_H */