#include "lince/ecs/ecs.h"


#ifdef LINCE_DEBUG
	#define LINCE_STATIC
#else
	#define LINCE_STATIC static
#endif

 
#define HASHMAP_INIT_SIZE 10

/** @brief Sets the bit corresponding to a component ID on a bitmask */
LINCE_STATIC void LinceECSSetMaskBit(LinceECSMask mask, uint32_t comp_id) {
	mask[comp_id / 64] |= ((uint64_t)1 << (comp_id % 64) );
}

/** @brief Unsets the bit corresponding to a component ID on a bitmask */
LINCE_STATIC void LinceECSUnsetMaskBit(LinceECSMask mask, uint32_t comp_id) {
	mask[comp_id / 64] &= ~((uint64_t)1 << (comp_id % 64));
}

/** @brief Returns true if a given bit is set on a bit mask */
LINCE_STATIC LinceBool LinceECSCheckMaskBit(LinceECSMask mask, uint32_t comp_id) {
	return !!(mask[comp_id / 64] & ((uint64_t)1 << (comp_id % 64)));
}

/** @brief Returns the index of a component store in an archetype */
LINCE_STATIC uint32_t LinceECSGetComponentStoreIndex(LinceECS* ecs, LinceECSMask mask, uint32_t comp_id) {
	hashmap_t* comp_archetypes = array_get(&ecs->component_index, (uint32_t)comp_id);
	return (uint32_t)(uint64_t)hashmap_getb(comp_archetypes, mask, sizeof(LinceECSMask));
}

/** @brief Returns the store where a component is located given its archetype */
LINCE_STATIC LinceECSComponentStore* LinceECSGetComponentStoreWithArch(LinceECS* ecs, LinceECSArchetype* arch, uint32_t comp_id) {
	uint32_t comp_store_id = LinceECSGetComponentStoreIndex(ecs, arch->mask, comp_id);
	return array_get(&arch->comp_stores, comp_store_id);
}

/** @brief Returns the store where a component is located in an archetype using its bitmask */
LINCE_STATIC LinceECSComponentStore* LinceECSGetComponentStoreWithMask(LinceECS* ecs, LinceECSMask mask, uint32_t comp_id) {
	uint64_t arch_id = (uint64_t)hashmap_getb(&ecs->archetype_map, mask, sizeof(LinceECSMask));
	LinceECSArchetype* arch = array_get(&ecs->archetypes, (uint32_t)arch_id);
	return LinceECSGetComponentStoreWithArch(ecs, arch, comp_id);
}

LINCE_STATIC LinceECSArchetype* LinceECSGetOrCreateArchetype(LinceECS* ecs, LinceECSMask mask) {

	uint64_t arch_id;
	LinceECSArchetype* arch;

	LinceBool has_key = hashmap_has_keyb(&ecs->archetype_map, mask, sizeof(LinceECSMask));
	if (has_key) {
		arch_id = (uint64_t)hashmap_getb(&ecs->archetype_map, mask, sizeof(LinceECSMask));
		arch = array_get(&ecs->archetypes, (uint32_t)arch_id);
		return arch;
	}

	array_push_back(&ecs->archetypes, NULL);
	arch = array_back(&ecs->archetypes);

	memmove(arch->mask, mask, sizeof(LinceECSMask));
	array_init(&arch->entity_ids, sizeof(LinceEntity));
	array_init(&arch->comp_stores, sizeof(LinceECSComponentStore));
	array_init(&arch->unused_slots, sizeof(uint32_t));
	array_init(&arch->unused_index, sizeof(uint32_t));

	for (uint32_t comp_id = 0, column = 0; comp_id != ecs->component_count; ++comp_id) {
		if (!LinceECSCheckMaskBit(mask, comp_id)) continue;

		// Initialise component stores
		uint32_t comp_size = *(uint32_t*)array_get(&ecs->component_sizes, comp_id);
		LinceECSComponentStore comp_store = { .id = comp_id, .element_size = comp_size };
		array_init(&comp_store.data, comp_store.element_size);
		array_push_back(&arch->comp_stores, &comp_store);
		
		// Update component_index entry
		hashmap_t* comp_archetypes = array_get(&ecs->component_index, comp_id);
		hashmap_setb(comp_archetypes, mask, sizeof(LinceECSMask), (void*)(uint64_t)column);

		column++;
	}
	
	// Update archetype_map
	arch_id = (uint64_t)(ecs->archetypes.size - 1);
	void* r = hashmap_setb(&ecs->archetype_map, mask, sizeof(LinceECSMask), (void*)arch_id);
	if (!r) return NULL;

	return arch;
}


/* Adds an entity to an archetype and returns its slot. Does not modify the entity record. */
LINCE_STATIC uint32_t LinceECSAddToArchetype(LinceECS* ecs, LinceEntity entity_id, LinceECSArchetype* arch) {
	
	LinceECSRecord* record = array_get(&ecs->entity_records, (uint32_t)entity_id);
	uint32_t row;
	static uint32_t USED = LinceTrue;

	// Find slot in target archetype
	if (arch->unused_slots.size > 0) {
		
		row = *(uint32_t*)array_back(&arch->unused_slots);
		array_set(&arch->entity_ids, &entity_id, row);
		array_set(&arch->unused_index, &USED, row);
		array_pop_back(&arch->unused_slots);

	} else {

		array_push_back(&arch->entity_ids, &entity_id);
		array_push_back(&arch->unused_index, &USED);
		row = arch->entity_ids.size - 1;

		// Create new space in component stores
		for (uint32_t i = 0; i != arch->comp_stores.size; ++i) {
			LinceECSComponentStore* store = array_get(&arch->comp_stores, i);
			array_push_back(&store->data, NULL);
		}
	}

	// Copy components to the new archetype
	LinceECSArchetype* old_arch = array_get(&ecs->archetypes, record->arch_id);
	uint32_t old_row = record->row;

	for (uint32_t i = 0; i != old_arch->comp_stores.size; ++i) {
		LinceECSComponentStore* old_store = array_get(&old_arch->comp_stores, i);
		uint32_t comp_id = old_store->id;
		uint32_t comp_size = old_store->element_size;

		// Skip if component is not present in the new archetype
		if (!LinceECSCheckMaskBit(arch->mask, comp_id)) {
			continue;
		}

		LinceECSComponentStore* new_store = LinceECSGetComponentStoreWithArch(ecs, arch, comp_id);
		void* p1 = array_get(&new_store->data, old_row);
		void* p2 = array_get(&new_store->data, row);
		memcpy(p2, p1, comp_size);
	}

	return row;
}

/* Removes an entity from an archetype */
LINCE_STATIC void LinceECSRemoveFromArchetype(LinceECS* ecs, LinceEntity entity_id) {

	LinceECSRecord* record = array_get(&ecs->entity_records, (uint32_t)entity_id);
	LinceECSArchetype* arch = array_get(&ecs->archetypes, record->arch_id);

	// Flag slot in old archetype as unused
	static uint32_t UNUSED = LinceFalse;
	array_push_back(&arch->unused_slots, &record->row);
	array_set(&arch->unused_index, &UNUSED, record->row);
}


LinceECS* LinceECSInit(LinceECS* ecs) {
	LINCE_ASSERT(ecs, "Null pointer");
	
	if (!ecs) return NULL;
	
	ecs->component_count = 0;
	ecs->entity_count = 0;
	ecs->user_data = NULL;
	array_init(&ecs->entity_records,    sizeof(LinceECSRecord));
	array_init(&ecs->component_sizes,   sizeof(uint32_t));
	array_init(&ecs->archetypes,        sizeof(LinceECSArchetype));
	array_init(&ecs->component_index,   sizeof(hashmap_t));
	array_init(&ecs->entity_pool,       sizeof(LinceEntity));
	hashmap_init(&ecs->archetype_map,   HASHMAP_INIT_SIZE);

	// Create the default archetype with no components
	// Located at index 0, the default value when initialising an entity record
	void* zero_arch = LinceECSGetOrCreateArchetype(ecs, (LinceECSMask) { 0 });
	if (!zero_arch) return NULL;

	LINCE_INFO("ECS initialised");

	return ecs;
}

void LinceECSUninit(LinceECS* ecs) {

	/* Free components stored in archetypes */
	for (uint32_t i = 0; i != ecs->archetypes.size; ++i) {
		LinceECSArchetype* arch = array_get(&ecs->archetypes, i);
		array_uninit(&arch->entity_ids);
		array_uninit(&arch->unused_slots);
		array_uninit(&arch->unused_index);
		for (uint32_t j = 0; j != arch->comp_stores.size; ++j) {
			LinceECSComponentStore* store = array_get(&arch->comp_stores, j);
			array_uninit(&store->data);
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
	ecs->component_count = 0;
	ecs->entity_count = 0;
	ecs->user_data = NULL;
}

// Create an entity
LinceEntity LinceECSNewEntity(LinceECS* ecs) {
	LinceEntity entity;
	LinceECSRecord *record, emtpy_record = { .flags = LinceECSFlags_Active };

	if (ecs->entity_pool.size > 0) {
		// Draw entity from pool of inactive ones
		entity = *(LinceEntity*)array_back(&ecs->entity_pool);
		array_pop_back(&ecs->entity_pool);
		record = array_get(&ecs->entity_records, (uint32_t)entity);
		memcpy(record, &emtpy_record, sizeof(LinceECSRecord));
	}
	else {
		entity = (LinceEntity)ecs->entity_records.size;
		array_push_back(&ecs->entity_records, &emtpy_record);
		record = array_back(&ecs->entity_records);
	}

	// Append to empty archetype
	LinceECSArchetype* zero_arch = array_get(&ecs->archetypes, 0);
	record->row = LinceECSAddToArchetype(ecs, entity, zero_arch);

	ecs->entity_count++;
	LINCE_INFO("ECS: Created new entity with ID %lu", entity);
	return entity;
}

// Removes an entity
void LinceECSDeleteEntity(LinceECS* ecs, LinceEntity entity) {
	if (!ecs) return;

	LinceECSRecord* record = array_get(&ecs->entity_records, (uint32_t)entity);
	if (!record) return;
	if (!(record->flags & LinceECSFlags_Active)) {
		LINCE_WARN("ECS: deleting inactive entity wiht ID %lu", entity);
		return;
	}

	// Flag its archetype slot as available
	LinceECSRemoveFromArchetype(ecs, entity);

	// Reset record
	memset(record, 0, sizeof(LinceECSRecord));
	
	// Add to entity pool
	array_push_back(&ecs->entity_pool, &entity);
	ecs->entity_count--;
	LINCE_INFO("ECS: Deleted entity with ID %lu", entity);
}


// Register new component and return its ID
uint32_t LinceECSNewComponent(LinceECS* ecs, uint32_t component_size) {
	LINCE_ASSERT(ecs, "Null pointer");
	LINCE_ASSERT(component_size > 0, "ECS: Zero-sized component");
	LINCE_ASSERT(ecs->component_count < LINCE_ECS_MAX_COMPONENTS,
		"ECS: Exceeded maximum number of components");
	
	uint32_t component_id = ecs->component_count;
	array_push_back(&ecs->component_sizes, &component_size);
	ecs->component_count++;
	
	hashmap_t arch_records;
	hashmap_init(&arch_records, 5);
	array_push_back(&ecs->component_index, &arch_records);
	
	LINCE_INFO("ECS: added new component of size %u with ID %u",
			component_size, component_id);
	return component_id;
}


void* LinceECSAddComponents(LinceECS* ecs, LinceEntity entity_id, uint32_t component_num, uint32_t* component_ids) {
	
	if (!ecs || !component_ids || component_num == 0) return NULL;
	
	LinceECSRecord* record = array_get(&ecs->entity_records, (uint32_t)entity_id);
	LINCE_ASSERT(record, "Entity %lu does not exist", entity_id);
	LINCE_ASSERT(record->flags & LinceECSFlags_Active, "Entity %lu is not active", entity_id);
	if (!record || !(record->flags & LinceECSFlags_Active)) return NULL;

	// Create new mask
	LinceECSMask new_mask;
	memcpy(new_mask, record->mask, sizeof(LinceECSMask));
	for (uint32_t i = 0; i != component_num; ++i) {
		uint32_t comp_id = component_ids[i];
		if (comp_id >= ecs->component_count) return NULL;
		if (LinceECSCheckMaskBit(record->mask, comp_id)) return NULL; // Component already present
		LinceECSSetMaskBit(new_mask, comp_id);
	}

	if (memcmp(record->mask, new_mask, sizeof(LinceECSMask)) == 0) {
		// Masks are identical, no new components added!
		return NULL;
	}

	// Fetch or create its new archetype
	LinceECSArchetype* new_arch = LinceECSGetOrCreateArchetype(ecs, new_mask);
	if (!new_arch) return NULL;
	uint64_t new_arch_id = (new_arch - (LinceECSArchetype*)ecs->archetypes.data);

	// Move between archetypes
	uint32_t new_row = LinceECSAddToArchetype(ecs, entity_id, new_arch);
	LinceECSRemoveFromArchetype(ecs, entity_id);

	// Update records
	memcpy(record->mask, new_mask, sizeof(LinceECSMask));
	record->arch_id = (uint32_t)new_arch_id;
	record->row = new_row;
	return ecs;
}


// Get a component of an entity
void* LinceECSGetComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id) {
	// Get entity record and its mask and archetype
	LinceECSRecord*    record = array_get(&ecs->entity_records, (uint32_t)entity_id);
	LINCE_ASSERT(record, "Entity with id %lu does not exist", entity_id);
	
	LinceECSArchetype* arch = array_get(&ecs->archetypes, record->arch_id);
	uint32_t row = record->row;
	LINCE_ASSERT(arch, "No archetype exists for entity %lu", entity_id);
	
	// Get column and row of component in archetype
	hashmap_t* comp_archetypes = array_get(&ecs->component_index, component_id);
	LINCE_ASSERT(comp_archetypes, "No archetypes exist with component %u", component_id);
	
	uint32_t column = (uint32_t)(uintptr_t)hashmap_getb(comp_archetypes, record->mask, sizeof(LinceECSMask));

	LinceECSComponentStore* comp_store = array_get(&arch->comp_stores, column);
	LINCE_ASSERT(comp_store, "Archetype does not have component store for component %u", component_id);
	
	void* comp_data = array_get(&comp_store->data, row);
	LINCE_ASSERT(comp_data, "Entity %lu does not have component %u", entity_id, component_id);

	return comp_data;
}

// Provide data for a component
void* LinceECSSetComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id, void* data) {
	if (!ecs) return NULL;
	
	void* dest = LinceECSGetComponent(ecs, entity_id, component_id);
	if (!dest || component_id >= ecs->component_count) return NULL;
	
	uint32_t size = *(uint32_t*)array_get(&ecs->component_sizes, component_id);
	if (data) {
		memmove(dest, data, size);
	} else {
		memset(dest, 0, size);
	}
	return dest;
}


/* Add a component to an entity and provide its data */
void* LinceECSEmplaceComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id, void* data) {
	void* success = LinceECSAddComponents(ecs, entity_id, 1, &component_id);
	if (!success) return NULL;
	return LinceECSSetComponent(ecs, entity_id, component_id, data);
}


void* LinceECSRemoveComponents(LinceECS* ecs, LinceEntity entity_id, uint32_t component_num, uint32_t* component_ids) {
	
	if (!ecs || !component_ids || component_num == 0) return NULL;

	LinceECSRecord* record = array_get(&ecs->entity_records, (uint32_t)entity_id);
	LINCE_ASSERT(record, "Entity %lu does not exist", entity_id);
	LINCE_ASSERT(record->flags & LinceECSFlags_Active, "Entity %lu is not active", entity_id);
	if (!record || !(record->flags & LinceECSFlags_Active)) return NULL;

	// Create new mask
	LinceECSMask new_mask;
	memcpy(new_mask, record->mask, sizeof(LinceECSMask));
	for (uint32_t i = 0; i != component_num; ++i) {
		uint32_t comp_id = component_ids[i];
		if (comp_id >= ecs->component_count) return NULL;
		if (!LinceECSCheckMaskBit(record->mask, comp_id)) return NULL; // Component already not present
		LinceECSUnsetMaskBit(new_mask, comp_id);
	}

	if (memcmp(record->mask, new_mask, sizeof(LinceECSMask)) == 0) {
		// Masks are identical, no components removed!
		return NULL;
	}

	// Fetch or create its new archetype
	LinceECSArchetype* new_arch = LinceECSGetOrCreateArchetype(ecs, new_mask);
	if (!new_arch) return NULL;
	uint64_t new_arch_id = (new_arch - (LinceECSArchetype*)ecs->archetypes.data);

	// Move between archetypes
	uint32_t new_row = LinceECSAddToArchetype(ecs, entity_id, new_arch);
	LinceECSRemoveFromArchetype(ecs, entity_id);

	// Update records
	memcpy(record->mask, new_mask, sizeof(LinceECSMask));
	record->arch_id = (uint32_t)new_arch_id;
	record->row = new_row;
	return ecs;

}

// True if an entity has the given component
LinceBool LinceECSHasComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id) {
	LINCE_ASSERT(ecs, "NULL pointer");
	LINCE_ASSERT(component_id < ecs->component_count, "Component %u does not exist", component_id);
	LinceECSRecord* record = array_get(&ecs->entity_records, (uint32_t)entity_id);
	LINCE_ASSERT(record, "Entity with id %lu does not exist", entity_id);
	return LinceECSCheckMaskBit(record->mask, component_id);
}

// Returns an array of the entities that have the requested components
array_t* LinceECSQuery(LinceECS* ecs, LinceECSMask mask, array_t* result) {

	for (uint32_t arch_id = 0; arch_id != ecs->archetypes.size; ++arch_id) {
		LinceECSArchetype* arch = array_get(&ecs->archetypes, arch_id);

		// Compare masks
		// Works with the zero-component archetype
		LinceBool match = LinceTrue;
		for (uint32_t comp_id = 0; comp_id != ecs->component_count; ++comp_id) {
			if (!LinceECSCheckMaskBit(mask, comp_id)) continue;
			if (!LinceECSCheckMaskBit(arch->mask, comp_id)) {
				match = LinceFalse;
				break;
			}
		}
		if (!match) continue;
		if (arch->entity_ids.size == 0) continue;

		// Collect entities from matching archetype
		for(uint32_t i = 0; i != arch->entity_ids.size; ++i){
			LinceBool used = *(LinceBool*)array_get(&arch->unused_index, i);
			if (!used) continue;
			LinceEntity* entity_id = array_get(&arch->entity_ids, i);
			array_push_back(result, entity_id);
		}

	}
	return result;
}


void LinceECSUpdate(LinceECS* ecs, float dt) {

	array_t query;
	array_init(&query, sizeof(LinceEntity));

	for (uint32_t i = 0; i != ecs->archetypes.size; ++i) {
		LinceECSArchetype* arch = array_get(&ecs->archetypes, i);
		if (!arch->on_update) continue;

		LinceECSQuery(ecs, arch->mask, &query);
		arch->on_update(ecs, &query);
		array_clear(&query);
	}

	array_uninit(&query);

}