#ifndef LINCE_COLLIDER_H
#define LINCE_COLLIDER_H

#include "cglm/vec2.h"

typedef enum LinceBoxColliderFlags{
    LinceBoxCollision_None  = 0x0,
    LinceBoxCollision_Right = 0x1,
    LinceBoxCollision_Left  = 0x2,
    LinceBoxCollision_Top   = 0x4,
    LinceBoxCollision_Bot   = 0x8,

    LinceBoxCollision_X = LinceBoxCollision_Right | LinceBoxCollision_Left,
    LinceBoxCollision_Y = LinceBoxCollision_Top   | LinceBoxCollision_Bot,

} LinceBoxColliderFlags;


typedef struct LinceBoxCollider{
	vec2 pos;  // center x,y
    vec2 size; // width, height
} LinceBoxCollider;


int LinceCollideBox(LinceBoxCollider* a, LinceBoxCollider* b);

#endif /* LINCE_COLLIDER_H */