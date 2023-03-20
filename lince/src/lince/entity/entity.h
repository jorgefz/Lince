#ifndef LINCE_ECS_H
#define LINCE_ECS_H


#include <lince/core/core.h>
#include <lince/core/memory.h>
#include <lince/containers/array.h>

#ifndef LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT
/** @brief Maximum number of components,
* defined as the number of 64-bit unsigned ints, where each bit is a component.
* 
* Examples:
*     one uint64_t -> 64  components
*     two uint64_t -> 128 components
* Default: 1 uint64_t, so max of 64 components
*/
#define LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT 1
#endif

/** @typedef LinceEntityMask
* @brief Entity mask type, where eacl bit tells you where a given entity
* has a specific component.
* The bit location also tells you the component ID.
*/
typedef uint64_t LinceEntityMask[LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT];

/** @enum LinceEntityState
* @brief Entity flags
*/
typedef enum LinceEntityState {
    LinceEntityState_Active = 0x1, ///< In use, not flagged for recycle
} LinceEntityState;

/** @struct LinceEntityRegistry
* @brief Holds the state of a set of entities in a cache-friendly way.
*/
typedef struct LinceEntityRegistry {
    uint32_t component_count;    ///< Number of defined components
    uint32_t max_components;     ///< Maximum number of components,
                                 ///< tweaked with LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT
    array_t  component_sizes;    ///< array<uint32_t> -> size in bytes of each defined component
    array_t  component_offsets;  ///< array<uint32_t> -> byte offsets between components in an entity

    uint32_t entity_size;     ///< size in bytes of an entity: sum all defined components
    uint32_t entity_count;    ///< number of loaded entities
    array_t entity_data;      ///< array<bytes> -> component data of all entities
    array_t entity_flags;     ///< array<LinceEntityState> -> further data in the form of flags
    array_t entity_masks;     ///< array<uint64_t> -> bit fields that indicate which component each entity has
    array_t entity_pool;      ///< array<uint32_t> -> entity IDs available to be re-used
} LinceEntityRegistry;


/** @brief Creates a registry that will be used to spawn entities.
*
* Takes as input the sizes of components to load as 32-bit unsigned integers,
* followed by the size in bytes of each component in the form
* of variadic arguments.
* The order of the components will define their IDs,
* e.g. from the first component with id = 0, to the last with id = N-1.
* Is is then useful to make an enum with components in the same order.
* Example code:
* ```c
* LinceEntityRegistry* reg;
* reg = LinceCreateEntityRegistry(
*     3, // load three components
*     sizeof(SpriteComponent), // Will have id = 0
*     sizeof(ColliderComponent), // Will have id = 1
*     sizeof(ShaderComponent), // Will have id = 2
*     ...
* );
* enum ComponentID { SpriteComponentID = 0, ColliderComponentID, ShaderComponentID };
* ```
* Returns NULL if the component count is invalid (zero)
* or if it failed to allocate memory.
*/
LinceEntityRegistry* LinceCreateEntityRegistry(uint32_t component_count, ...);
void LinceDestroyEntityRegistry(LinceEntityRegistry* reg);

/** @brief Creates a new entity and returns its ID. */
uint32_t LinceCreateEntity(LinceEntityRegistry* reg);

/** @brief Deletes an entity and flags its ID for recycling */
void LinceDeleteEntity(LinceEntityRegistry* reg, uint32_t entity_id);

/** @brief Provides data for a component to an entity
* @param reg Entity registry,
* @param entity_id Entity to which add a component. Must have already been created.
* @param component_id Component to provide to the entity. The ID corresponds to
* the order in which its size passed to `LinceCreateEntityRegistry`.
* @param data Component data. Its size was defined when the registry was created.
*/
void LinceAddEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id, void* data);

/** @brief Returns true if a given entity has the specified component */
LinceBool LinceHasEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id);

/** @brief Returns the component data from an entity */
void* LinceGetEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id);

/** @brief Removes a component from an entity */
void LinceRemoveEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id);

/** @brief Searches for entities that have all of the specified components.
* @param reg Entity registry
* @param query Array that will hold the entitu IDs that satisfy the query.
*       Must be initialised for uint32_t elements!
* @param component_count Number of components to search for in entities.
*       This is followed by the component IDs in the form of variadic arguments.
* @returns The number of entities that match the query.
*/
uint32_t LinceQueryEntities(LinceEntityRegistry* reg, array_t* query, uint32_t component_count, ...);

#endif /* LINCE_ECS_H */