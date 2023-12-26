#include "lince/ecs/ecs.h"


#define HASHMAP_INIT_SIZE 10

/** @brief Sets the bit corresponding to a component ID on a bitmask */
static void LinceECSSetMaskBit(LinceECSMask mask, uint32_t comp_id) {
	mask[comp_id / 64] |= ((uint64_t)1 << (comp_id % 64) );
}

/** @brief Unsets the bit corresponding to a component ID on a bitmask */
static void LinceECSUnsetMaskBit(LinceECSMask mask, uint32_t comp_id) {
	mask[comp_id / 64] &= ~((uint64_t)1 << (comp_id % 64));
}

/** @brief Returns true if a given bit is set on a bit mask */
static LinceBool LinceECSCheckMaskBit(LinceECSMask mask, uint32_t comp_id) {
	return mask[comp_id / 64] & ((uint64_t)1 << (comp_id % 64));
}


LinceECS* LinceECSInit(LinceECS* ecs) {
	LINCE_ASSERT(ecs, "Null pointer");
	if (!ecs) return NULL;

	LINCE_ASSERT(!ecs->initialised, "ECS already initialised");
	if (ecs->initialised) return NULL;

	array_init(&ecs->entity_records,    sizeof(LinceECSRecord));
	array_init(&ecs->component_sizes,   sizeof(uint32_t));
	array_init(&ecs->archetypes,        sizeof(LinceECSArchetype));
	array_init(&ecs->component_index,   sizeof(hashmap_t));
	array_init(&ecs->entity_pool,       sizeof(LinceEntity));
	hashmap_init(&ecs->archetype_map,   HASHMAP_INIT_SIZE);
	
	ecs->initialised = LinceTrue;

	return ecs;
}

void LinceECSUninit(LinceECS* ecs) {

	/* Free components stored in archetypes */
	for (uint32_t i = 0; i != ecs->archetypes.size; ++i) {
		LinceECSArchetype* arch = array_get(&ecs->archetypes, i);
		array_uninit(&arch->entity_ids);
		for (uint32_t j = 0; j != arch->comp_stores.size; ++j) {
			LinceECSComponentStore* comp_store = array_get(&arch->comp_stores, j);
			array_uninit(&comp_store->data);
		}
		array_uninit(&arch->comp_stores);
	}

	/* Free maps stored in component indices */
	for (uint32_t i = 0; i != ecs->component_index.size; ++i) {
		hashmap_t* archetypes = array_get(&ecs->component_index, i);
		hashmap_uninit(archetypes);
	}

	array_uninit(&ecs->entity_records);
	array_uninit(&ecs->component_sizes);
	array_uninit(&ecs->archetypes);
	array_uninit(&ecs->component_index);
	array_uninit(&ecs->entity_pool);
	hashmap_uninit(&ecs->archetype_map);
	ecs->initialised = LinceFalse;
}

// Create an entity
LinceEntity LinceECSNewEntity(LinceECS* ecs) {
	LinceEntity entity;

	if (ecs->entity_pool.size > 0) {
		entity = *(LinceEntity*)array_back(&ecs->entity_pool);
		array_pop_back(&ecs->entity_pool);
		LinceECSRecord* record = array_get(&ecs->entity_records, entity);
		record->flags |= LinceECSFlags_Active;
		return entity;
	}

	entity = ecs->entity_records.size;
	LinceECSRecord new_record = { .flags = LinceECSFlags_Active };
	array_push_back(&ecs->entity_records, &new_record);
	return entity;
}

// Removes an entity
void LinceECSDeleteEntity(LinceECS* ecs) {

}


// Register new component and return its ID
uint32_t LinceECSNewComponent(LinceECS* ecs, uint32_t component_size) {
	LINCE_ASSERT(ecs, "Null pointer");
	if (!ecs) return -1;

	LINCE_ASSERT(component_size > 0, "Zero-sized component");
	if (component_size == 0) return -1;

	uint32_t component_id = ecs->component_sizes.size;
	array_push_back(&ecs->component_sizes, &component_size);
	
	hashmap_t arch_records;
	hashmap_init(&arch_records, 5);
	array_push_back(&ecs->component_index, &arch_records);
	
	LINCE_INFO("ECS: added new component of size %u with ID %u",
			component_size, component_id);
	return component_id;
}


// Adds a component to an entity
void* LinceECSAddComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id, void* data) {
	// Check component exists
	// Check entity exists

	LinceECSRecord* record = array_get(&ecs->entity_records, entity_id);
	LinceECSMask* old_mask = &record->mask;

	// Create new mask
	uint32_t idx = component_id / 64;
	uint32_t bit = component_id % 64;
	LinceECSMask new_mask;
	memcpy(&new_mask, old_mask, sizeof(LinceECSMask));
	new_mask[idx] |= ((uint64_t)1 << bit);

	// Fetch new archetype
	LinceECSArchetype* new_archetype = hashmap_has_keyb(&ecs->archetype_map, new_mask, sizeof(LinceECSMask));

	if (!new_archetype) {
		// new_archetype = CreateArchetype(ecs, new_mask)
	}

	// move entity data to new archetype
	// remove from old archetype
	
	// return
}


// Get a component of an entity
void* LinceECSGetComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id) {
	// Get entity record and its mask and archetype
	LinceECSRecord*    record = array_get(&ecs->entity_records, entity_id);
	LINCE_ASSERT(record, "Entity with id %lu does not exist", entity_id);
	
	LinceECSMask*      mask = &record->mask;
	LinceECSArchetype* arch   = record->archetype;
	LinceEntity        row    = record->row;
	LINCE_ASSERT(arch, "No archetype exists for entity %lu", entity_id);
	
	// Get column and row of component in archetype
	hashmap_t* comp_archetypes = array_get(&ecs->component_index, component_id);
	LINCE_ASSERT(comp_archetypes, "No archetypes exist with component %u", component_id);
	
	uint32_t column = hashmap_getb(comp_archetypes, &mask, sizeof(LinceECSMask));

	LinceECSComponentStore* comp_store = array_get(&arch->comp_stores, column);
	LINCE_ASSERT(comp_store, "Archetype does not have component store for component %u", component_id);
	
	void* comp_data = array_get(&comp_store->data, row);
	LINCE_ASSERT(comp_data, "Entity %lu does not have component %u", entity_id, component_id);

	return comp_data;
}

// Removes a component from an entity
void* LinceECSRemoveComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id);

// True if an entity has the given component
LinceBool LinceECSHasComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id) {
	LINCE_ASSERT(ecs, "NULL pointer");
	LINCE_ASSERT(component_id < ecs->component_sizes.size, "Component %u does not exist", component_id);
	LinceECSRecord* record = array_get(&ecs->entity_records, entity_id);
	LINCE_ASSERT(record, "Entity with id %lu does not exist", entity_id);
	return LinceECSCheckMaskBit(record->mask, component_id);
}

// Returns an array of the entities that have the requested components
array_t* LinceECSQuery(LinceECS* ecs, uint32_t comp_num, uint32_t* comp_ids);
