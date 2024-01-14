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


/** Number of 64-bit slots in component bitmasks. Maximum number of components as a multiple of 64. */
#ifndef LINCE_ECS_COMPONENT_SLOTS
	#define LINCE_ECS_COMPONENT_SLOTS 1
#endif

/** Maximum number of components */
#define LINCE_ECS_MAX_COMPONENTS ((uint32_t)(LINCE_ECS_COMPONENT_SLOTS) * 64)

/** Bitmask that represents a combination of components */
typedef uint64_t LinceECSMask[LINCE_ECS_COMPONENT_SLOTS];

struct LinceECS; /* Forward declaration */

/** @brief Function type for ECS systems
* @param ecs ECS state
* @param dt Delta time
* @param entities Array<LinceEntity> array with all entities that have the requested components
*/
typedef void (*LinceECSSystem)(struct LinceECS* ecs, float dt, array_t* entities);

/** Flags indicating the state of an entity
* @todo store flags in the top 32 bits of the entity ID
*/
typedef enum LinceECSFlags {
	LinceECSFlags_Active = 0x1
} LinceECSFlags;

/** Stores the data of one component of all entities with the same components */
typedef struct LinceECSComponentStore {
	array_t data;		   	///< array<element size>, stores data of a component for entities with an archetype
	uint32_t id;			///< ID of the stored component
	uint32_t element_size;	///< Size in bytes of the stored component
} LinceECSComponentStore;

/** Stores the data of entities with the same components */
typedef struct LinceECSArchetype {
	array_t        comp_stores;  ///< array<LinceECSComponentStore>
	array_t        entity_ids;   ///< array<LinceEntity>, entities with this archetype
	array_t		   unused_index; ///< array<LinceBool> Bools for whether slots are used (1) or unused (0).
	array_t		   unused_slots; ///< array<uint32_t> Indices of empty slots in component data.
	LinceECSMask   mask;		 ///< bitmask signature of the archetype
	LinceECSSystem on_update;    ///< Callback provided with all the entities (across all archetypes) that have the components in this archetype.
								 ///< All such systems are run when LinceECSUpdate is called.
} LinceECSArchetype;

/** Stores metadata of an entity */
typedef struct LinceECSRecord {
	LinceECSMask       mask;	///< Component mask of the entity
	LinceECSFlags      flags;	///< State flags
	uint32_t		   row;     ///< Index inside component stores where entity data is located
	uint32_t		   arch_id; ///< Index of its archetype.
} LinceECSRecord;

/** Main state */
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

} LinceECS;


/** Make static functions accessible in debug mode */
#ifdef LINCE_DEBUG
void LinceECSSetMaskBit(LinceECSMask mask, uint32_t comp_id);
void LinceECSUnsetMaskBit(LinceECSMask mask, uint32_t comp_id);
LinceBool LinceECSCheckMaskBit(LinceECSMask mask, uint32_t comp_id);
LinceECSArchetype* LinceECSGetOrCreateArchetype(LinceECS* ecs, LinceECSMask mask);
#endif

/** @brief Initialise ECS state
* @param ecs Input state, must point to valid memory.
* @returns The address of the input state if successful, and NULL otherwise.
*/
LinceECS* LinceECSInit(LinceECS* ecs);

/** @brief Deallocate memory and reset state */
void LinceECSUninit(LinceECS* ecs);

/** @brief Create an entity and return its ID */
LinceEntity LinceECSNewEntity(LinceECS* ecs);

/** @brief Remove an entity */
void LinceECSDeleteEntity(LinceECS* ecs, LinceEntity entity);

/** @brief Register new component and returns its ID
* @param ecs ECS state
* @param component_size size of the new component in bytes, must be greater than zero.
* @returns component ID with value between 0 and LINCE_ECS_MAX_COMPONENTS if successful, or -1 otherwise. 
*/
uint32_t LinceECSNewComponent(LinceECS* ecs, uint32_t component_size);

/** @brief Registers components with an entity
* @param ecs ECS state
* @param entity_id Entity to which to add components
* @param component_num Number of components to add. Must be greater than zero.
* @param component_ids Array of IDs of components to add. All components must have been registered.
* @returns Non-null pointer if successul, NULL otherwise.
* @note Space for the input components will be allocated, but their data will not be set. Use LinceECSSetComponent to provide the data.
*/
void* LinceECSAddComponents(LinceECS* ecs, LinceEntity entity_id, uint32_t component_num, uint32_t* components_ids);

/** @brief Retrieve the data of a component
* @param ecs ECS state
* @param entity_id Entity from which to retrieve a component
* @param component_id ID of component whose data to return
* @returns pointer to location where the component's data is stored, or NULL if unsuccessul.
* @note to test only for the presence of a component, it is faster to use LinceECSHasComponent.
*/
void* LinceECSGetComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id);

/** @brief Provide or overwrite the data of a component
* @param ecs ECS state
* @param entity_id ID of entity whose component to set
* @param component_id ID of component to modify
* @param data New data for component. If NULL, the component is zeroed.
* @returns Pointer to component data, or NULL if unsuccessful.
*/
void* LinceECSSetComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id, void* data);

/** @brief Add a component to an entity and provide its data.
* @param ecs ECS state
* @param entity_id ID of entity whose component to add
* @param component_id ID of component to modify
* @param data Data for component. If NULL, the component is zeroed.
* @returns Pointer to component data, or NULL if unsuccessful.
* @note If adding several components, it is faster to add them together with LinceECSAddComponent, and then set their data with LinceECSSetComponent.
*/
void* LinceECSEmplaceComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id, void* data);

/** @brief Remove components from an entity
* @param ecs ECS state
* @param entity_id ID of entity whose component(s) to remove
* @param component_num Number of components to remove. Must be greater than zero.
* @param component_ids Array of IDs of components to remove. All components must have been registered.
* @returns non-null pointer if successful, or NULL if unsuccessful.
*/
void* LinceECSRemoveComponents(LinceECS* ecs, LinceEntity entity_id, uint32_t component_num, uint32_t* component_ids);

/** @brief Returns true if an entity has a given component.
* @param ecs ECS state
* @param entity_id ID of entity
* @param component_id ID of component whose existence to check
* @returns LinceTrue if the entity has the component, and LinceFalse otherwise.
*/
LinceBool LinceECSHasComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id);

/** @brief Returns an array of the entities that have the requested components.
* @param ecs ECS state
* @param result Initialised array to which the query results will be written
* @param component_num Number of components to query. Must be greater than zero.
* @param component_ids Array of IDs of components to query. All components must have been registered.
*/
array_t* LinceECSQuery(LinceECS* ecs, array_t* result, uint32_t comp_count, uint32_t* comp_ids);

/** @brief Register a callback that is run on entities that have a given set of components.
* @param ecs ECS state
* @param system Function callback to assign to the given combination of components.
* @param comp_num Number of components in signature.
* @param Components that entities must have to be passed to the system.
* @returns non-null pointer if successful, NULL otherwise.
* @note Systems are called sequentially when LinceECSUpdate is run.
*/
void* LinceECSAddSystem(LinceECS* ecs, LinceECSSystem system, uint32_t comp_num, uint32_t* comp_ids);

/** @brief Advances the world by one time step and runs the system callbacks.
* @param ecs ECS state
* @param dt delta time
*/
void LinceECSUpdate(LinceECS* ecs, float dt);



#endif /* LINCE_ECS_H */