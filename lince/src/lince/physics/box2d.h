#ifndef LINCE_BOX2D_H
#define LINCE_BOX2D_H

#include <lince/core/core.h>
#include <lince/containers/array.h>
#include <lince/entity/entity.h>

/** @enum LinceBox2DFlags
* @brief Settings and state for a box collider.
*/
typedef enum LinceBox2DFlags {
    // Settings
    LinceBox2D_Bounce = 0x1, ///< (unused) flips direction on collision
    LinceBox2D_Static = 0x2, ///< Hints that collider will not move. Skips collision check.
    // State
    LinceBox2D_CollisionX = 0x4, ///< Indicates collision on the x axis
    LinceBox2D_CollisionY = 0x8, ///< Indicates collision on the y axis
    LinceBox2D_Collision = LinceBox2D_CollisionX | LinceBox2D_CollisionY,
} LinceBox2DFlags;

/** @struct LinceBox2D
* @brief Holds the spatial data for a rectangular non-rotated colliders.
*/
typedef struct LinceBox2D {
    float x, y;    ///< 2D position
    float w, h;    ///< Size
    float dx, dy;  ///< displacement when moving, update as needed before computing collisions
    LinceBox2DFlags flags; ///< State flags. See `LinceBox2DFlags`.
} LinceBox2D;

/** @brief Returns true if a box collider contains another */
LinceBool LinceBox2DContains(LinceBox2D* box1, LinceBox2D* box2);

/** @brief Returns true if two box colliders are in contact */
LinceBool LinceBox2DCollides(LinceBox2D* box1, LinceBox2D* box2);

/** @brief Computes collisions between all entities in a registry
* that have a BoxCollider component.
* @todo Improve neighbour search algorithm - spatial indexing?
*/
void LinceCalculateEntityCollisions(LinceEntityRegistry* reg, array_t* entities, int box_component_id);

#endif /* LINCE_BOX2D_H */