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

/* Integral type for Entity IDs */
typedef uint64_t LinceEntity;


/// Number of 64-bit slots in entity bitmasks. Maximum number of components as a multiple of 64.
#ifndef LINCE_ECS_COMPONENT_SLOTS
	#define LINCE_ECS_COMPONENT_SLOTS 1
#endif

/// Maximum number of components
#define LINCE_ECS_MAX_COMPONENTS ((uint32_t)(LINCE_ECS_COMPONENT_SLOTS) * 64)


// TODO: switch to struct
typedef uint64_t LinceECSMask[LINCE_ECS_COMPONENT_SLOTS];

/* Avoid typedef array type, use struct, can be casted to int pointer */
/*
typedef struct LinceECSMask {
	uint64_t slots[LINCE_ECS_MAX_COMPONENTS_FACTOR];
} LinceECSMask;
*/

struct LinceECS; /* Forward declaration */

/* Function type for system callbacks */
typedef void (*LinceECSSystem)(struct LinceECS* ecs, float dt, array_t* entities);

/* Flags indicating the state of an entity */
// TODO: store flags in the top 32 bits of the entity ID
typedef enum LinceECSFlags {
	LinceECSFlags_Active = 0x1
} LinceECSFlags;

typedef struct LinceECSComponentStore {
	array_t data;		   	///< array<element size>, stores data of a component for entities with an archetype
	uint32_t id;			///< ID of the stored component
	uint32_t element_size;	///< Size in bytes of the stored component
} LinceECSComponentStore;

typedef struct LinceECSArchetype {
	array_t        comp_stores;  ///< array<LinceECSComponentStore>
	array_t        entity_ids;   ///< array<LinceEntity>, entities with this archetype
	array_t		   unused_index; ///< array<LinceBool> Bools for whether slots are used (1) or unused (0).
	array_t		   unused_slots; ///< array<uint32_t> Indices of empty slots in component data.
	LinceECSMask   mask;		 ///< bitmask signature of the archetype
	LinceECSSystem on_update;    ///< Callback provided with all the entities (across all archetypes) that have the components in this archetype.
								 ///< All such systems are run when LinceECSUpdate is called.
} LinceECSArchetype;

typedef struct LinceECSRecord {
	LinceECSMask       mask;	///< Component mask of the entity
	LinceECSFlags      flags;	///< State flags
	uint32_t		   row;     ///< Index inside component stores where entity data is located
	uint32_t		   arch_id; ///< Index of its archetype.
} LinceECSRecord;

typedef struct LinceECS {
	// Data
	array_t   entity_records;  ///< array<Record> Indexed by entity_id, holds masks and flags
	array_t   component_sizes; ///< array<uint32_t>  Indexed by comp_id, holds component sizes
	array_t   archetypes;      ///< array<Archetype> Indexed by arch_id, holds archetypes
	
	// Relations
	array_t   component_index; ///< array< hashmap<Mask,uint32_t> > Indexed by comp_id, holds archetypes associated with each component
	hashmap_t archetype_map;   ///< map<Mask, uint32_t> Links a type mask to the index of the archetype for that type
	array_t   entity_pool;     ///< array<LinceEntity> unused entities

	// Generic
	void*     user_data;
	uint32_t  component_count; ///< Number of components
	uint32_t  entity_count;    ///< Number of active entities
	array_t   query_result;    ///< Caches the array for ECSQuery so that it is only initialised once
	// hashmap_t query_cache;     ///< Hashmap< Array<arch_id> > Arrays of archetypes that match the search query for a given mask.

	/*
	
	// -- Fetch query cache
	cache = query_cache[query_mask];
	for arch_id in cache {
		query_result.add( archetypes[arch_id].entities );
	}
	
	// -- Remake query
	cache = query_cache[query_mask]
	for arch_id, arch in archetypes {
		if(arch.mask & query_mask){
			cache.add( arch_id );
			query_result.add( arch.entities )
		}
	}

	// -- Signal redo query (when new archetype is added)
	// No need to do this every time an entity moves archetypes
	// because we do not cache matching entities, but matching archetypes

	*/

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

/** @brief Create an entity and return its ID */
LinceEntity LinceECSNewEntity(LinceECS* ecs);

/** @brief Remove an entity */
void LinceECSDeleteEntity(LinceECS* ecs, LinceEntity entity);

/** @brief Register new component and returns its ID */
uint32_t LinceECSNewComponent(LinceECS* ecs, uint32_t component_size);

/** @brief Add components to an entity */
void* LinceECSAddComponents(LinceECS* ecs, LinceEntity entity_id, uint32_t component_num, uint32_t* components_ids);

/** @brief Retrieve a component from an entity */
void* LinceECSGetComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id);

/** @brief Provide or overwrite the data of a component on an entity
* If the data is not provided (NULL), the component is zeroed.
*/
void* LinceECSSetComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id, void* data);

/** @brief Add one component to an entity and provide its data.
* If the data is NULL, the component is zeroed.
*/
void* LinceECSEmplaceComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id, void* data);

/** @brief Remove components from an entity */
void* LinceECSRemoveComponents(LinceECS* ecs, LinceEntity entity_id, uint32_t component_num, uint32_t* component_ids);

/** @brief Returns true if an entity has a given component */
LinceBool LinceECSHasComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id);

/** @brief Returns an array of the entities that have the requested components */
array_t* LinceECSQuery(LinceECS* ecs, array_t* result, uint32_t comp_count, uint32_t* comp_ids);

/** @brief Register a callback that is run on entities that have a given set of components */
void* LinceECSAddSystem(LinceECS* ecs, LinceECSSystem callback, uint32_t comp_num, uint32_t* comp_ids);

/** @brief Advances the world by one time step and runs the system callbacks */
void LinceECSUpdate(LinceECS* ecs, float dt);






#endif /* LINCE_ECS_H */