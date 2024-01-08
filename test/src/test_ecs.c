#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <lince/ecs/ecs.h>
#include "benchmark.h"


static void ecs_debug_print(LinceECS* ecs) {
	if (!ecs) return;

	printf("\n");
	printf(" --- ECS ---\n");
	printf(" Entities: %u (%u active)\n", ecs->entity_records.size, ecs->entity_count);
	printf(" Components: %u\n", ecs->component_count);
	printf(" Archetypes: %u\n", ecs->archetypes.size);
	printf("\n");

	// Entities
	for (uint32_t i = 0; i != ecs->entity_records.size; ++i) {
		LinceECSRecord* record = array_get(&ecs->entity_records, i);

		printf(" -- Entity %u\n", i);
		printf("	Active: %s\n", (record->flags & LinceECSFlags_Active) ? "True" : "False");
		printf("	Mask:   ");
		for (uint32_t j = 0; j != 64; ++j) {
			printf("%lu", LinceECSCheckMaskBit(record->mask, j) );
		}
		printf("\n");
		printf("	Arch:   %d\n", (int)record->arch_id);
		printf("	Row:    %u\n", record->row);
	}
	printf("\n");

	// Archetypes
	for (uint32_t i = 0; i != ecs->archetypes.size; ++i) {
		LinceECSArchetype* arch = array_get(&ecs->archetypes, i);

		printf(" -- Archetype %u\n", i);
		printf("	Mask:   ");
		for (uint32_t j = 0; j != 64; ++j) {
			printf("%lu", LinceECSCheckMaskBit(arch->mask, j));
		}
		printf("\n");

		printf("	Entity IDs: ");
		for (uint32_t j = 0; j != arch->entity_ids.size; ++j) {
			LinceBool used = *(LinceBool*)array_get(&arch->unused_index, j);
			if (used) printf("%u ", *(uint32_t*)array_get(&arch->entity_ids, j));
			else      printf("- ");
		}
		if (arch->entity_ids.size == 0) {
			printf("<Empty>");
		}
		printf("\n");
	}

	printf("\n");
}


void test_ecs_mask(void** state) {
	(void)state;

	assert_int_equal(LINCE_ECS_COMPONENT_SLOTS, 1);
	assert_int_equal(sizeof(LinceECSMask), sizeof(uint64_t));

	LinceECSMask mask = { 0 };

	LinceECSSetMaskBit(mask, 0);
	assert_int_equal(mask[0], 1);
	assert_int_equal(LinceECSCheckMaskBit(mask, 0), 1);

	LinceECSUnsetMaskBit(mask, 0);
	assert_int_equal(mask[0], 0);
	assert_int_equal(LinceECSCheckMaskBit(mask, 0), 0);

	LinceECSSetMaskBit(mask, 63);
	assert_int_equal(mask[0], ((uint64_t)1 << 63));
	assert_int_equal(LinceECSCheckMaskBit(mask, 63), 1);
	
	LinceECSSetMaskBit(mask, 25);
	LinceECSSetMaskBit(mask, 50);
	assert_int_equal(mask[0], ((uint64_t)1 << 63) | ((uint64_t)1 << 25) | ((uint64_t)1 << 50));
	assert_int_equal(LinceECSCheckMaskBit(mask, 25), 1);
	assert_int_equal(LinceECSCheckMaskBit(mask, 50), 1);
}


void test_ecs(void** state) {
	(void)state;

	/* --- Initialisation --- */
	LinceECS ecs;
	void* r;
	r = LinceECSInit(&ecs);

	assert_non_null(r);
	assert_ptr_equal(&ecs, r);
	assert_int_equal(ecs.entity_records.size, 0);
	assert_int_equal(ecs.component_sizes.size, 0);
	assert_int_equal(ecs.archetypes.size, 1);
	assert_int_equal(ecs.component_index.size, 0);
	assert_int_equal(ecs.entity_pool.size, 0);
	assert_non_null(ecs.archetype_map.table);

	// Default archetype with no components
	LinceECSArchetype* zero_arch = array_get(&ecs.archetypes, 0);
	assert_memory_equal(zero_arch->mask, (LinceECSMask) { 0 }, sizeof(LinceECSMask));


	/* --- Creating components --- */
	struct Comp0 { int a; };
	struct Comp1 { float x, y; };
	struct Comp2 { int a; float x, y, z[2]; };

	uint32_t comp_sizes[] = { sizeof(struct Comp0), sizeof(struct Comp1), sizeof(struct Comp2)};
	uint32_t comp_ids[3];

	comp_ids[0] = LinceECSNewComponent(&ecs, comp_sizes[0]);
	
	assert_int_equal(ecs.component_sizes.size, 1);
	assert_int_equal(ecs.component_count, 1);
	assert_int_equal(comp_ids[0], 0);
	assert_int_equal(comp_sizes[0], *(uint32_t*)array_get(&ecs.component_sizes, comp_ids[0]));
	assert_int_equal(ecs.component_index.size, 1);
	assert_int_equal( ((hashmap_t*)array_get(&ecs.component_index, 0))->size > 0, 1);
	assert_non_null(((hashmap_t*)array_get(&ecs.component_index, 0))->table);

	comp_ids[1] = LinceECSNewComponent(&ecs, comp_sizes[1]);
	comp_ids[2] = LinceECSNewComponent(&ecs, comp_sizes[2]);

	assert_int_equal(ecs.component_sizes.size, 3);
	assert_int_equal(ecs.component_count, 3);
	assert_int_equal(comp_ids[1], 1);
	assert_int_equal(comp_ids[2], 2);
	assert_int_equal(comp_sizes[1], *(uint32_t*)array_get(&ecs.component_sizes, comp_ids[1]));
	assert_int_equal(comp_sizes[2], *(uint32_t*)array_get(&ecs.component_sizes, comp_ids[2]));
	assert_int_equal(ecs.component_index.size, 3);
	assert_non_null(((hashmap_t*)array_get(&ecs.component_index, 1))->table);
	assert_non_null(((hashmap_t*)array_get(&ecs.component_index, 2))->table);

	/* --- Creating archetypes --- */
	LinceECSMask arch_mask = { 0 };
	LinceECSSetMaskBit(arch_mask, 0);
	LinceECSSetMaskBit(arch_mask, 2);
	LinceECSArchetype* arch = LinceECSGetOrCreateArchetype(&ecs, arch_mask);
	uint32_t arch_id = (uint32_t)(uint64_t)(arch - (LinceECSArchetype*)ecs.archetypes.data);

	assert_non_null(arch);
	assert_int_equal(arch_id, ecs.archetypes.size-1);
	assert_int_equal(ecs.archetypes.size, 2);
	assert_ptr_equal(arch, array_get(&ecs.archetypes, 1));
	
	assert_memory_equal(arch->mask, arch_mask, sizeof(LinceECSMask));
	assert_int_equal(arch->comp_stores.size,  2);
	assert_int_equal(arch->entity_ids.size,   0);
	assert_int_equal(arch->unused_index.size, 0);
	
	// Component stores
	uint32_t col_comp0 = 0, col_comp2 = 1;
	LinceECSComponentStore *store0, *store2;
	store0 = array_get(&arch->comp_stores, col_comp0);
	assert_non_null(store0);
	assert_int_equal(store0->id, comp_ids[0]);
	assert_int_equal(store0->element_size, comp_sizes[0]);
	assert_int_equal(store0->data.size, 0);

	store2 = array_get(&arch->comp_stores, col_comp2);
	assert_non_null(store2);
	assert_int_equal(store2->id, comp_ids[2]);
	assert_int_equal(store2->element_size, comp_sizes[2]);
	assert_int_equal(store2->data.size, 0);

	// Archetype map
	assert_int_equal(arch_id, hashmap_getb(&ecs.archetype_map, arch_mask, sizeof(LinceECSMask)));

	// Component index
	hashmap_t* comp_arch0 = array_get(&ecs.component_index, comp_ids[0]);
	assert_non_null(comp_arch0);
	uint32_t col0 = (uint32_t)(uint64_t)hashmap_getb(comp_arch0, arch_mask, sizeof(LinceECSMask));
	assert_int_equal(col0, col_comp0); // index of comp store in archetype

	hashmap_t* comp_arch2 = array_get(&ecs.component_index, comp_ids[2]);
	assert_non_null(comp_arch2);
	uint32_t col2 = (uint32_t)(uint64_t)hashmap_getb(comp_arch2, arch_mask, sizeof(LinceECSMask));
	assert_int_equal(col2, col_comp2); // index of comp store in archetype

	/* --- Creating and deleting entities --- */
	LinceEntity entity0;
	entity0 = LinceECSNewEntity(&ecs);

	assert_int_equal(entity0, 0);
	assert_int_equal(ecs.entity_count, 1);
	LinceECSRecord* record0 = array_get(&ecs.entity_records, (uint32_t)entity0);
	assert_non_null(record0);
	assert_int_equal(record0->flags & LinceECSFlags_Active, 1);
	assert_int_equal(record0->row, 0);
	assert_memory_equal(record0->mask, (LinceECSMask) { 0 }, sizeof(LinceECSMask));
	assert_int_equal(record0->arch_id, 0); // Default empty archetype
	
	// Delete the entity
	LinceECSDeleteEntity(&ecs, entity0);

	assert_int_equal(ecs.entity_count, 0);
	assert_int_equal(ecs.entity_pool.size, 1);
	assert_int_equal(*(LinceEntity*)array_get(&ecs.entity_pool, 0), entity0);
	assert_int_equal(record0->flags, 0);

	// Remake the entity
	entity0 = LinceECSNewEntity(&ecs);

	assert_int_equal(ecs.entity_count, 1);
	assert_int_equal(ecs.entity_pool.size, 0);
	assert_int_equal(record0->flags, LinceECSFlags_Active);

	// Make more entities
	LinceEntity entity1 = LinceECSNewEntity(&ecs);
	LinceEntity entity2 = LinceECSNewEntity(&ecs);

	/*
	Fetch record again because previous ponter becomes useless when
	the capacity of the array is extended (realloc)
	*/
	record0 = array_get(&ecs.entity_records, (uint32_t)entity0);
	LinceECSRecord* record1 = array_get(&ecs.entity_records, (uint32_t)entity1);
	LinceECSRecord* record2 = array_get(&ecs.entity_records, (uint32_t)entity2);

	assert_int_equal(ecs.entity_count, 3);
	assert_non_null(record1);
	assert_non_null(record2);
	assert_int_equal(record1->flags, LinceECSFlags_Active);
	assert_int_equal(record2->flags, LinceECSFlags_Active);

	/* Add components */
	r = LinceECSAddComponents(&ecs, entity0, 1, (uint32_t[]){comp_ids[0]});

	assert_non_null(r);
	assert_int_equal(LinceECSCheckMaskBit(record0->mask, 0), 1);
	assert_int_equal(LinceECSCheckMaskBit(record0->mask, 1), 0);
	assert_int_equal(LinceECSCheckMaskBit(record0->mask, 2), 0);
	assert_int_equal(record0->arch_id != -1, 1);
	LinceECSArchetype* arch0 = array_get(&ecs.archetypes, record0->arch_id);
	assert_non_null(arch0);
	assert_int_equal(LinceECSCheckMaskBit(arch0->mask, 0), 1);
	assert_int_equal(LinceECSCheckMaskBit(arch0->mask, 1), 0);
	assert_int_equal(LinceECSCheckMaskBit(arch0->mask, 2), 0);
	assert_int_equal(arch0->comp_stores.size, 1);
	assert_int_equal(arch0->entity_ids.size, 1);
	assert_int_equal(entity0, *(LinceEntity*)array_get(&arch0->entity_ids,0));
	
	// Check wrong input
	r = LinceECSAddComponents(NULL, entity0, 1, (uint32_t[]) { comp_ids[1] });
	assert_null(r); // No ECS provided
	r = LinceECSAddComponents(&ecs, entity0, 1, NULL);
	assert_null(r); // No components provided
	r = LinceECSAddComponents(&ecs, entity0, 0, (uint32_t[]) { comp_ids[1] });
	assert_null(r); // Zero components provided

#ifdef LINCE_RELEASE
	r = LinceECSAddComponents(&ecs, entity0, 1, (uint32_t[]) { 100 });
	assert_null(r); // Component ID does not exist
	r = LinceECSAddComponents(&ecs, 100, 1, (uint32_t[]) { comp_ids[1] });
	assert_null(r); // Entity does not exist
	r = LinceECSAddComponents(&ecs, entity0, 1, (uint32_t[]) { comp_ids[0] });
	assert_null(r); // Added no new components
#endif

	// Retrieve pointer to component
	void* comp0_loc = LinceECSGetComponent(&ecs, entity0, comp_ids[0]);

	assert_non_null(comp0_loc);
	arch0 = array_get(&ecs.archetypes, record0->arch_id);
	LinceECSComponentStore* store = array_get(&arch0->comp_stores, 0);
	void* comp0_ptr = array_get(&store->data, 0);
	assert_ptr_equal(comp0_loc, comp0_ptr);

	// Set component data (zeroed)
	comp0_ptr = LinceECSSetComponent(&ecs, entity0, comp_ids[0], NULL);

	assert_non_null(comp0_ptr);
	assert_ptr_equal(comp0_loc, comp0_ptr);
	int zero_pool[4] = { 0 }; // larger than Comp0
	assert_memory_equal(comp0_ptr, &zero_pool, comp_sizes[0]);

	// Set component data (with values)
	struct Comp0 comp0 = { .a = 15 };
	comp0_ptr = LinceECSSetComponent(&ecs, entity0, comp_ids[0], &comp0);
	
	assert_memory_equal(comp0_ptr, &comp0, comp_sizes[0]);

	// Add another component and check it migrated archetypes
	struct Comp1 comp1 = { .x = 5.0, .y = -3.0 };
	uint32_t prev_arch_id = record0->arch_id;
	comp0_ptr = LinceECSEmplaceComponent(&ecs, entity0, comp_ids[1], &comp1);
	
	assert_non_null(comp0_ptr);
	assert_int_not_equal(record0->arch_id, prev_arch_id);
	assert_memory_not_equal(
		array_get(&ecs.archetypes, record0->arch_id),
		array_get(&ecs.archetypes, prev_arch_id),
		sizeof(LinceECSArchetype)
	);

	/* Remove components */

	// Remove one component
	r = LinceECSRemoveComponents(&ecs, entity0, 1, (uint32_t[]){ comp_ids[1] });

	assert_non_null(r);
	assert_int_equal(record0->arch_id, prev_arch_id);

	// Remove all components
	r = LinceECSRemoveComponents(&ecs, entity0, 1, (uint32_t[]) { comp_ids[0] });

	assert_non_null(r);
	assert_int_equal(record0->arch_id, 0);
	
	/* Query entities */

	LinceECSMask query_mask = { 0 };
	array_t query;
	array_init(&query, sizeof(LinceEntity));

	// Test zero-component archetype
	// No entities should have components
	r = LinceECSQuery(&ecs, query_mask, &query);
	assert_null(r);
	
	array_clear(&query);

	// Add some components to make it interesting
	LinceECSAddComponents(&ecs, entity0, 3, comp_ids);          // Entity 0: 0 1 2
	LinceECSAddComponents(&ecs, entity1, 2, (uint32_t[]){0,2}); // Entity 1: 0   2
	LinceECSAddComponents(&ecs, entity2, 1, (uint32_t[]){0,1}); // Entity 2: 0 1   

	// Find entities with components 0, 1, and 2 (only entity 0)
	LinceECSSetMaskBit(query_mask, 0);
	LinceECSSetMaskBit(query_mask, 1);
	LinceECSSetMaskBit(query_mask, 2);
	r = LinceECSQuery(&ecs, query_mask, &query);

	assert_non_null(r);
	assert_ptr_equal(&query, r);
	assert_int_equal(query.size, 1);
	assert_memory_equal(query.data, &entity0, query.size * query.element_size);

	// Find entities with component 0 (all entities, despite having different archetypes)
	array_clear(&query);
	LinceECSUnsetMaskBit(query_mask, 1);
	LinceECSUnsetMaskBit(query_mask, 2);
	r = LinceECSQuery(&ecs, query_mask, &query);

	assert_non_null(r);
	assert_ptr_equal(&query, r);
	assert_int_equal(query.size, 3);
	assert_memory_equal(query.data, ((LinceEntity[]){1,2,0}), query.size * query.element_size);

	// Find entities with component 4 (no entities)
	array_clear(&query);
	uint32_t new_comp_id = LinceECSNewComponent(&ecs, sizeof(int));

	LinceECSUnsetMaskBit(query_mask, 0);
	LinceECSSetMaskBit(query_mask, new_comp_id);
	r = LinceECSQuery(&ecs, query_mask, &query);

	assert_non_null(r);
	assert_ptr_equal(&query, r);
	assert_int_equal(query.size, 0);

	array_uninit(&query);

	LinceECSUninit(&ecs);
}




void System100(LinceECS* ecs, float dt, array_t* entities) {
	assert_non_null(ecs);
	assert_float_equal(dt, 1.0, 1e-6);
	assert_int_equal(entities->size, 3);
	assert_memory_equal(entities->data, ((LinceEntity[]){0, 1, 2}), entities->size*entities->element_size);
}

void System101(LinceECS* ecs, float dt, array_t* entities) {
	assert_non_null(ecs);
	assert_float_equal(dt, 1.0, 1e-6);
	assert_int_equal(entities->size, 2);
	assert_memory_equal(entities->data, ((LinceEntity[]){0,1}), entities->size*entities->element_size);
}

void System111(LinceECS* ecs, float dt, array_t* entities) {
	assert_non_null(ecs);
	assert_float_equal(dt, 1.0, 1e-6);
	assert_int_equal(entities->size, 1);
	assert_memory_equal(entities->data, ((LinceEntity[]){0}), entities->size*entities->element_size);
}

void System0001(LinceECS* ecs, float dt, array_t* entities) {
	assert_non_null(ecs);
	assert_float_equal(dt, 1.0, 1e-6);
	assert_int_equal(entities->size, 0);
}




void test_ecs_system(void** state) {
	(void)state;

	// Prepare state
	void* r;
	LinceECS ecs;
	LinceECSInit(&ecs);

	LinceEntity entity_ids[3];
	uint32_t comp_ids[4];
	uint32_t comp_sizes[4] = { 8, 16, 32, 64 };

	for (uint32_t i = 0; i != 4; ++i) {
		comp_ids[i] = LinceECSNewComponent(&ecs, comp_sizes[i]);
	}


	for (uint32_t i = 0; i != 3; ++i) {
		entity_ids[i] = LinceECSNewEntity(&ecs);
	}

	LinceECSAddComponents(&ecs, entity_ids[0], 3, comp_ids);
	LinceECSAddComponents(&ecs, entity_ids[1], 2, (uint32_t[]){0,2});
	LinceECSAddComponents(&ecs, entity_ids[2], 2, (uint32_t[]){0,1});

	// Test functions
	r = LinceECSAddSystem(&ecs, NULL, 0, NULL);
	assert_null(r);

	r = LinceECSAddSystem(&ecs, System100, 1, &comp_ids[0]);
	assert_non_null(r);

	r = LinceECSAddSystem(&ecs, System101, 2, (uint32_t[]) { comp_ids[0], comp_ids[2] });
	assert_non_null(r);

	r = LinceECSAddSystem(&ecs, System111, 3, comp_ids);
	assert_non_null(r);

	r = LinceECSAddSystem(&ecs, System0001, 1, &comp_ids[3]);
	assert_non_null(r);

	LinceECSUpdate(&ecs, 1.0);

	// ecs_debug_print(&ecs);

	LinceECSUninit(&ecs);
}

