#ifndef LINCE_ECS_H
#define LINCE_ECS_H

#include "lince/containers/array.h"
#include "lince/containers/hashmap.h"
#include "lince/core/core.h"


#ifndef LINCE_ECS_MAX_COMPONENTS_FACTOR
#define LINCE_ECS_MAX_COMPONENTS_FACTOR 1
#endif

typedef uint64_t LinceECSMask[LINCE_ECS_MAX_COMPONENTS_FACTOR];

struct LinceECS; // Forward declaration

typedef void (*LinceECSSystem)(struct LinceECS* ecs, array_t* entity_ids);

typedef enum LinceECSFlags {
	LinceEntityState_Active = 0x1
} LinceECSFlags;

typedef struct LinceECSComponentStore {
	array_t  data;
	uint32_t id;
	uint32_t element_size;
} LinceECSComponentStore;

typedef struct LinceECSArchetype {
	array_t      comp_stores;
	array_t      entity_ids;
	LinceECSMask mask;
} LinceECSArchetype;

typedef struct LinceECSRecord {
	LinceECSMask       mask;
	LinceECSFlags      flags;
	LinceECSArchetype* archetype;
	uint32_t		   index;
} LinceECSRecord;

typedef struct LinceECS {
	array_t   entity_records; ///< array<Record>
	array_t   comp_sizes;     ///< array<uint32_t>
	array_t   comp_arch;      ///< array<Archetype>
	hashmap_t arch_map;       ///< map<Mask, Arch>

	array_t   entity_pool;    ///< array<uint32_t> unused entities
	// systems
	void*     user_data;
} LinceECS;


LinceECS* LinceECSInit(LinceECS* ecs);

void LinceECSUninit(LinceECS* ecs);

// Create an entity
uint32_t LinceECSNewEntity(LinceECS* ecs);

// Removes an entity
void LinceECSRemoveEntity(LinceECS* ecs);

// Register new component and returns its ID
uint32_t LinceECSNewComponent(LinceECS* ecs, uint32_t component_size);

// Adds a component to an entity
void* LinceECSAddComponent(LinceECS* ecs, uint32_t entity_id, uint32_t component_id, void* data);

// Get a component of an entity
void* LinceECSGetComponent(LinceECS* ecs, uint32_t entity_id, uint32_t component_id);

// Removes a component from an entity
void* LinceECSRemoveComponent(LinceECS* ecs, uint32_t entity_id, uint32_t component_id);

// True if an entity has the given component
LinceBool LinceECSHasComponent(LinceECS* ecs, uint32_t entity_id, uint32_t component_id);

// Returns an array of the entities that have the requested components
array_t* LinceECSQuery(LinceECS* ecs, uint32_t comp_num, uint32_t* comp_ids);

// void LinceECSSetSystemCallback(LinceECS* ecs, const char* callback_tag, LinceECSSystem* callback, uint32_t comp_num, uint32_t* comp_ids);

// #define LinceECSSetSystem(ecs, callback, comp_num, comp_ids) LinceECSSetSystemCallback(ecs, #callback, callback, comp_num, comp_ids)

// Move the world by one time step, runs the system callbacks
void LinceECSProgress(LinceECS* ecs, float dt);






#endif /* LINCE_ECS_H */