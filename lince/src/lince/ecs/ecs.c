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
	LinceECSArchetype* arch = hashmap_getb(&ecs->archetype_map, mask, sizeof(LinceECSMask));
	return LinceECSGetComponentStoreWithArch(ecs, arch, comp_id);
}

LINCE_STATIC LinceECSArchetype* LinceECSGetOrCreateArchetype(LinceECS* ecs, LinceECSMask mask) {

	LinceECSArchetype* arch = hashmap_getb(&ecs->archetype_map, mask, sizeof(LinceECSMask));
	if (arch) return arch;

	array_push_back(&ecs->archetypes, NULL);
	arch = array_back(&ecs->archetypes);

	memmove(arch->mask, mask, sizeof(LinceECSMask));
	array_init(&arch->entity_ids, sizeof(LinceEntity));
	array_init(&arch->comp_stores, sizeof(LinceECSComponentStore));
	array_init(&arch->unused_slots, sizeof(uint32_t));
	
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
	hashmap_setb(&ecs->archetype_map, mask, sizeof(LinceECSMask), arch);
	
	return arch;
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
	
	LINCE_INFO("ECS initialised");

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
	ecs->component_count = 0;
	ecs->entity_count = 0;
	ecs->user_data = NULL;
}

// Create an entity
LinceEntity LinceECSNewEntity(LinceECS* ecs) {
	LinceEntity entity;

	if (ecs->entity_pool.size > 0) {
		entity = *(LinceEntity*)array_back(&ecs->entity_pool);
		array_pop_back(&ecs->entity_pool);
		LinceECSRecord* record = array_get(&ecs->entity_records, (uint32_t)entity);
		memset(record, 0, sizeof(LinceECSRecord));
		record->flags |= LinceECSFlags_Active;
		ecs->entity_count++;
		LINCE_INFO("ECS: Created recycled entity with ID %lu", entity);
		return entity;
	}

	entity = (LinceEntity)ecs->entity_records.size;
	LinceECSRecord new_record = { .flags = LinceECSFlags_Active };
	array_push_back(&ecs->entity_records, &new_record);
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

	// Remove from archetype
	if (record->archetype) {
		// TODO
	}

	// Reset record
	record->flags = 0;
	record->row = 0;
	memset(record->mask, 0, sizeof(LinceECSMask));

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


// Adds a component to an entity
void* LinceECSAddComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id, void* data) {
	LINCE_ASSERT(component_id < ecs->component_count, "Component %u does not exist", component_id);

	LinceECSRecord* record = array_get(&ecs->entity_records, (uint32_t)entity_id);
	LINCE_ASSERT(record, "Entity %lu does not exist", entity_id);
	LINCE_ASSERT(record->flags & LinceECSFlags_Active, "Entity %lu is not active", entity_id);

	// If component already present, only replace its data
	if (LinceECSCheckMaskBit(record->mask, component_id)) {
		void* component = LinceECSGetComponent(ecs, entity_id, component_id);
		uint32_t component_size = *(uint32_t*)array_get(&ecs->component_sizes, component_id);
		memmove(component, data, component_size);
		return component;
	}

	// Create new mask
	LinceECSMask new_mask;
	memcpy(new_mask, record->mask, sizeof(LinceECSMask));
	LinceECSSetMaskBit(new_mask, component_id);

	// Fetch or create its new archetype
	LinceECSArchetype* new_arch = LinceECSGetOrCreateArchetype(ecs, new_mask);
	uint32_t new_row;
	
	if (new_arch->unused_slots.size > 0) {
		new_row = *(uint32_t*)array_back(&new_arch->unused_slots);
		array_set(&new_arch->entity_ids, &entity_id, new_row);
		array_pop_back(&new_arch->unused_slots);
	}
	else {
		array_push_back(&new_arch->entity_ids, &entity_id);
		new_row = new_arch->entity_ids.size - 1;
	}

	// Copy components to new archetype
	for (uint32_t i = 0; i != record->archetype->comp_stores.size; ++i) {
		LinceECSComponentStore* old_comp_store = array_get(&record->archetype->comp_stores, i);
		uint32_t comp_id   = old_comp_store->id;
		uint32_t comp_size = old_comp_store->element_size;

		LinceECSComponentStore* new_comp_store = LinceECSGetComponentStoreWithArch(ecs, new_arch, comp_id);
		void* p1 = array_get(&old_comp_store->data, record->row);
		void* p2 = array_get(&new_comp_store->data, new_row);
		memcpy(p2, p1, comp_size);
	}

	// Copy new component
	LinceECSComponentStore* comp_store = LinceECSGetComponentStoreWithArch(ecs, new_arch, component_id);
	array_set(&comp_store->data, data, new_row);
	void* comp_data = array_get(&comp_store->data, new_row);

	// Flag slot in old archetype as unused
	array_push_back(&record->archetype->unused_slots, &record->row);

	// Update records with archetype and mask
	memcpy(record->mask, new_mask, sizeof(LinceECSMask));
	record->archetype = new_arch;
	record->row = (LinceEntity)new_row;
	return comp_data;
}


void* LinceECSAddComponents(LinceECS* ecs, LinceEntity entity_id, uint32_t component_num, uint32_t* components_ids) {
	
	if (!ecs || !components_ids || component_num == 0) return NULL;
	
	LinceECSRecord* record = array_get(&ecs->entity_records, (uint32_t)entity_id);
	LINCE_ASSERT(record, "Entity %lu does not exist", entity_id);
	LINCE_ASSERT(record->flags & LinceECSFlags_Active, "Entity %lu is not active", entity_id);
	if (!record || !(record->flags & LinceECSFlags_Active)) return NULL;

	// Create new mask
	LinceECSMask new_mask;
	memcpy(new_mask, record->mask, sizeof(LinceECSMask));
	for (uint32_t i = 0; i != component_num; ++i) {
		uint32_t comp_id = components_ids[i];
		if (comp_id >= ecs->component_count) return NULL;
		LinceECSSetMaskBit(new_mask, comp_id);
	}

	if (memcmp(record->mask, new_mask, sizeof(LinceECSMask)) == 0) {
		// Masks are identical, no new components added!
		return NULL;
	}

	// Fetch or create its new archetype
	LinceECSArchetype* new_arch = LinceECSGetOrCreateArchetype(ecs, new_mask);
	if (!new_arch) return NULL;

	uint32_t new_row;

	if (new_arch->unused_slots.size > 0) {
		new_row = *(uint32_t*)array_back(&new_arch->unused_slots);
		array_set(&new_arch->entity_ids, &entity_id, new_row);
		array_pop_back(&new_arch->unused_slots);
	} else {
		array_push_back(&new_arch->entity_ids, &entity_id);
		new_row = new_arch->entity_ids.size - 1;
		// Create new slots in component stores
		for (uint32_t i = 0; i != new_arch->comp_stores.size; ++i) {
			LinceECSComponentStore* store = array_get(&new_arch->comp_stores, i);
			array_push_back(&store->data, NULL);
		}
	}

	if (!record->archetype) {
		// No previous archetype
		// Only update records with archetype and mask
		memcpy(record->mask, new_mask, sizeof(LinceECSMask));
		record->archetype = new_arch;
		record->row = new_row;
		return ecs;
	}

	// Copy components from old to new archetype
	for (uint32_t i = 0; i != record->archetype->comp_stores.size; ++i) {
		LinceECSComponentStore* old_store = array_get(&record->archetype->comp_stores, i);
		uint32_t comp_id = old_store->id;
		uint32_t comp_size = old_store->element_size;

		LinceECSComponentStore* new_store = LinceECSGetComponentStoreWithArch(ecs, new_arch, comp_id);
		void* p1 = array_get(&new_store->data, record->row);
		void* p2 = array_get(&new_store->data, new_row);
		memcpy(p2, p1, comp_size);
	}

	// Copy new component
	// LinceECSComponentStore* comp_store = LinceECSGetComponentStoreWithArch(ecs, new_arch, component_id);
	// array_set(&comp_store->data, data, new_row);
	// void* comp_data = array_get(&comp_store->data, new_row);

	// Flag slot in old archetype as unused
	array_push_back(&record->archetype->unused_slots, &record->row);

	// Update records with archetype and mask
	memcpy(record->mask, new_mask, sizeof(LinceECSMask));
	record->archetype = new_arch;
	record->row = new_row;
	return ecs;
}


// Get a component of an entity
void* LinceECSGetComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id) {
	// Get entity record and its mask and archetype
	LinceECSRecord*    record = array_get(&ecs->entity_records, (uint32_t)entity_id);
	LINCE_ASSERT(record, "Entity with id %lu does not exist", entity_id);
	
	LinceECSArchetype* arch = record->archetype;
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


// Removes a component from an entity
void* LinceECSRemoveComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id);

// True if an entity has the given component
LinceBool LinceECSHasComponent(LinceECS* ecs, LinceEntity entity_id, uint32_t component_id) {
	LINCE_ASSERT(ecs, "NULL pointer");
	LINCE_ASSERT(component_id < ecs->component_count, "Component %u does not exist", component_id);
	LinceECSRecord* record = array_get(&ecs->entity_records, (uint32_t)entity_id);
	LINCE_ASSERT(record, "Entity with id %lu does not exist", entity_id);
	return LinceECSCheckMaskBit(record->mask, component_id);
}

// Returns an array of the entities that have the requested components
array_t* LinceECSQuery(LinceECS* ecs, uint32_t comp_num, uint32_t* comp_ids);
