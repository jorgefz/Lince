#ifndef LINCE_BOX_COLLIDER_H
#define LINCE_BOX_COLLIDER_H

#include <lince/core/core.h>
#include <lince/containers/array.h>
#include <lince/entity/entity.h>

typedef enum LinceBoxColliderFlags {
    // Settings
    LinceBoxCollider_Bounce = 0x1, // flips direction on collision
    LinceBoxCollider_Static = 0x2, // Hints that collider will not move. Skips collision check.
    // State
    LinceBoxCollider_CollisionX = 0x4,
    LinceBoxCollider_CollisionY = 0x8,
    LinceBoxCollider_Collision = LinceBoxCollider_CollisionX | LinceBoxCollider_CollisionY,
} LinceBoxColliderFlags;

typedef struct LinceBoxCollider {
    float x, y;    // position
    float w, h;    // size
    float dx, dy;  // displacement when moving, update as needed before computing collisions
    LinceBoxColliderFlags flags; // mode for collision reflection, flag for just collided, etc.
} LinceBoxCollider;

LinceBool LinceBoxCollides(LinceBoxCollider* rect1, LinceBoxCollider* rect2);

void LinceCalculateEntityCollisions(LinceEntityRegistry* reg, array_t* entities, int box_component_id);

#endif /* LINCE_BOX_COLLIDER_H */