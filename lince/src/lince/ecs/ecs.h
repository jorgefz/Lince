#ifndef LINCE_ECS_H
#define LINCE_ECS_H


#include <lince/core/core.h>
#include <lince/core/memory.h>
#include <lince/containers/array.h>

/*
Maximum number of components.
Defined as number of 64-bit ints, where each bit is a component.
e.g.:
    one uint64_t -> 64  components
    two uint64_t -> 128 components
*/
#ifndef LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT
#define LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT 1 // max 64 components
#endif

typedef uint64_t LinceEntityMask[LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT];

typedef enum LinceEntityState {
    LinceEntityState_Active = 0x1, // better active
} LinceEntityState;

typedef struct LinceEntityRegistry {
    uint32_t component_count;    // Number of defined components
    uint32_t max_components;     // Maximum number of components,
                                 // tweaked with LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT
    array_t  component_sizes;    // array<uint32_t> -> size in bytes of each defined component
    array_t  component_offsets;  // array<uint32_t> -> byte offsets between components in an entity

    uint32_t entity_size;     // size in bytes of an entity: sum all defined components
    uint32_t entity_count;    // number of loaded entities
    array_t entity_data;      // array<bytes> -> component data of all entities
    array_t entity_flags;     // array<LinceEntityState> ->
    array_t entity_masks;     // array<uint64_t> -> bit fields that indicate which component each entity has
    array_t entity_pool;      // array<uint32_t> -> entity IDs available to be used
} LinceEntityRegistry;


/*
Creates a registry that will be used to spawn entities.
Takes as input the sizes of components to load as 32-bit unsigned integers, e.g.:

    LinceEntityRegistry* reg;
    reg = LinceCreateEntityRegistry(
        3, // load three components
        sizeof(SpriteComponent),
        sizeof(ColliderComponent),
        sizeof(TimerComponent),
        ...
    );

Returns NULL if the component count is invalid (zero)
or if it failed to allocate memory.
*/
LinceEntityRegistry* LinceCreateEntityRegistry(uint32_t component_count, ...);
void LinceDestroyEntityRegistry(LinceEntityRegistry* reg);

/* Creates a new entity and returns its ID. */
uint32_t LinceCreateEntity(LinceEntityRegistry* reg);

/* Sets up an entity ID for recycling */
void LinceDeleteEntity(LinceEntityRegistry* reg, uint32_t entity_id);

/* Provides data for a component to an entity */
void LinceAddEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id, void* data);

/* Returns true if a given entity has the specified component */
LinceBool LinceHasEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id);

/* Returns the component data from an entity */
void* LinceGetEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id);

/* Removes a component from an entity */
void LinceDeleteEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id);

/*
Searches for entities that have the given components.
The provided component IDs must be 32-bit unsigned integers.
The resulting entity list is rturned through the provided array.
The return value is the number of entities that match the query.
*/
uint32_t LinceQueryEntities(LinceEntityRegistry* reg, array_t* query, uint32_t component_count, ...);

#endif /* LINCE_ECS_H */