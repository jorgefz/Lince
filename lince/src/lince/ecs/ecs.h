/*


 Arch
|-----|       Cols _______________
|  A  | -----> A: |_______________| Rows
|-----|            __________________
|     | -----> A: |__________________|
| ABC |    |-> B: |__________________|
|     |    |-> C: |__________________|
|-----|


GET: Entity E -> Archetype A -> Component Column C -> Component Row R

E -> from input, gives you mask
A -> from a map that links a mask to the archetype
C -> from some
R -> from some array indexed by input comp_id to get a record with the archetyes
	with that component and the corresponding row within

GET(entity, component)
	record = EntityRecords[entity]
	mask = record.mask
	archetype = record.archetype
	row = record.row
	column = ComponentIndex[component][mask]
	
	return archetype.components[column][row]

QUERY(components)
	for a in archetypes:
		


*/




#ifndef LINCE_ECS_H
#define LINCE_ECS_H

#include "lince/containers/array.h"
#include "lince/containers/hashmap.h"
#include "lince/core/core.h"


typedef uint64_t LinceEntity;

#ifndef LINCE_ECS_MAX_COMPONENTS_FACTOR
#define LINCE_ECS_MAX_COMPONENTS_FACTOR 1
#endif

typedef uint64_t LinceECSMask[LINCE_ECS_MAX_COMPONENTS_FACTOR];

struct LinceECS; // Forward declaration

typedef void (*LinceECSSystem)(struct LinceECS* ecs, array_t* entity_ids);

typedef enum LinceECSFlags {
	LinceECSFlags_Active = 0x1
} LinceECSFlags;

typedef struct LinceECSComponentStore {
	array_t  data;
	uint32_t id;
	uint32_t element_size;
} LinceECSComponentStore;

typedef struct LinceECSArchetype {
	array_t        comp_stores;  ///< array< LinceECSComponentStore >
	array_t        entity_ids;   ///< array<LinceEntity>
	array_t		   unused_slots; ///< Empty slots in component data, to be reused
								 ///< Correspond to indices in entity_ids and comp_stores
	LinceECSMask   mask;		 ///< bitmask signature of the archetype
	LinceECSSystem on_update;    ///< system callbacks
} LinceECSArchetype;

typedef struct LinceECSRecord {
	LinceECSMask       mask;
	LinceECSFlags      flags;
	LinceEntity		   row;
	LinceECSArchetype* archetype;
} LinceECSRecord;

typedef struct LinceECS {
	// Data
	array_t   entity_records;  ///< array<Record> Indexed by entity_id, holds masks and flags
	array_t   component_sizes; ///< array<uint32_t>  Indexed by comp_id, holds component sizes
	array_t   archetypes;      ///< array<Archetype> Indexed by arch_id, holds archetypes
	
	// Relations
	array_t   component_index; ///< array< hashmap<Mask,uint32_t> > Indexed by comp_id, holds archetypes associated with each component
	hashmap_t archetype_map;   ///< map<Mask, Archetype*> Links a type mask to the archetype for that type
	array_t   entity_pool;     ///< array<uint32_t> unused entities

	void*     user_data;
	uint32_t  component_count; ///< Number of components

} LinceECS;


/* Make static functions accessible in debug mode */
#ifdef LINCE_DEBUG
void LinceECSSetMaskBit(LinceECSMask mask, uint32_t comp_id);
void LinceECSUnsetMaskBit(LinceECSMask mask, uint32_t comp_id);
LinceBool LinceECSCheckMaskBit(LinceECSMask mask, uint32_t comp_id);
LinceECSArchetype* LinceECSGetOrCreateArchetype(LinceECS* ecs, LinceECSMask mask);
#endif


LinceECS* LinceECSInit(LinceECS* ecs);

void LinceECSUninit(LinceECS* ecs);

// Create an entity
LinceEntity LinceECSNewEntity(LinceECS* ecs);

// Removes an entity
void LinceECSDeleteEntity(LinceECS* ecs);

// Register new component and returns its ID
uint32_t LinceECSNewComponent(LinceECS* ecs, uint32_t component_size);

// Adds a component to an entity
void* LinceECSAddComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id, void* data);

// Get a component of an entity
void* LinceECSGetComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id);

// Removes a component from an entity
void* LinceECSRemoveComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id);

// True if an entity has the given component
LinceBool LinceECSHasComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id);

// Returns an array of the entities that have the requested components
array_t* LinceECSQuery(LinceECS* ecs, uint32_t comp_num, uint32_t* comp_ids);

// void LinceECSSetSystemCallback(LinceECS* ecs, const char* callback_tag, LinceECSSystem* callback, uint32_t comp_num, uint32_t* comp_ids);

// #define LinceECSSetSystem(ecs, callback, comp_num, comp_ids) LinceECSSetSystemCallback(ecs, #callback, callback, comp_num, comp_ids)

// Move the world by one time step, runs the system callbacks
void LinceECSUpdate(LinceECS* ecs, float dt);






#endif /* LINCE_ECS_H */