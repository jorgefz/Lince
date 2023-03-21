#ifndef LINCE_BOX_COLLIDER_H
#define LINCE_BOX_COLLIDER_H

#include <lince/core/core.h>
#include <lince/containers/array.h>
#include <lince/entity/entity.h>

/** @enum LinceBoxColliderFlags
* @brief Settings and state for a box collider.
*/
typedef enum LinceBoxColliderFlags {
    // Settings
    LinceBoxCollider_Bounce = 0x1, ///< (unused) flips direction on collision
    LinceBoxCollider_Static = 0x2, ///< Hints that collider will not move. Skips collision check.
    // State
    LinceBoxCollider_CollisionX = 0x4, ///< Indicates collision on the x axis
    LinceBoxCollider_CollisionY = 0x8, ///< Indicates collision on the y axis
    LinceBoxCollider_Collision = LinceBoxCollider_CollisionX | LinceBoxCollider_CollisionY,
} LinceBoxColliderFlags;

/** @struct LinceBoxCollider
* @brief Holds the spatial data for a rectangular non-rotated colliders.
*/
typedef struct LinceBoxCollider {
    float x, y;    ///< 2D position
    float w, h;    ///< Size
    float dx, dy;  ///< displacement when moving, update as needed before computing collisions
    LinceBoxColliderFlags flags; ///< State flags. See `LinceBoxColliderFlags`.
} LinceBoxCollider;

/** @brief Returns true if a box collider contains another */
LinceBool LinceBoxColliderContains(LinceBoxCollider* box1, LinceBoxCollider* box2);

/** @brief Returns true if two box colliders are in contact */
LinceBool LinceBoxCollides(LinceBoxCollider* rect1, LinceBoxCollider* rect2);

/** @brief Computes collisions between all entities in a registry
* that have a BoxCollider component.
* @todo Improve neighbour search algorithm - spatial indexing?
*/
void LinceCalculateEntityCollisions(LinceEntityRegistry* reg, array_t* entities, int box_component_id);

#endif /* LINCE_BOX_COLLIDER_H */