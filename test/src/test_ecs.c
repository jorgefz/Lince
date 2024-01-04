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

	for (uint32_t i = 0; i != ecs->entity_records.size; ++i) {
		LinceECSRecord* record = array_get(&ecs->entity_records, i);

		printf(" -- Entity %u\n", i);
		printf("	Active: %u\n", record->flags & LinceECSFlags_Active);
		printf("	Mask:   ");
		for (uint32_t j = 0; j != 64; ++j) {
			printf("%lu", LinceECSCheckMaskBit(record->mask, j) );
		}
		printf("\n");
		if (record->archetype) {
			uint32_t arch_id = (record->archetype - ecs->archetypes.data);
			printf("	Arch:   %u (0x%p)\n", arch_id, record->archetype);
		} else {
			printf("	Arch:   <NULL>\n");
		}
		printf("	Row:    %u\n", record->row);
	}

	printf("\n");

}


void test_ecs_mask(void** state) {

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

	/* --- Initialisation --- */
	LinceECS ecs;
	void* r;
	r = LinceECSInit(&ecs);

	assert_non_null(r);
	assert_ptr_equal(&ecs, r);
	assert_int_equal(ecs.entity_records.size, 0);
	assert_int_equal(ecs.component_sizes.size, 0);
	assert_int_equal(ecs.archetypes.size, 0);
	assert_int_equal(ecs.component_index.size, 0);
	assert_int_equal(ecs.entity_pool.size, 0);
	assert_non_null(ecs.archetype_map.table);

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

	assert_non_null(arch);
	assert_int_equal(ecs.archetypes.size, 1);
	assert_ptr_equal(arch, array_get(&ecs.archetypes, 0));
	
	assert_memory_equal(arch->mask, arch_mask, sizeof(LinceECSMask));
	assert_int_equal(arch->comp_stores.size,  2);
	assert_int_equal(arch->entity_ids.size,   0);
	assert_int_equal(arch->unused_slots.size, 0);
	
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
	assert_ptr_equal(arch, hashmap_getb(&ecs.archetype_map, arch_mask, sizeof(LinceECSMask)));

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
	assert_null(record0->archetype);

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
	assert_non_null(record0->archetype);
	assert_int_equal(LinceECSCheckMaskBit(record0->archetype->mask, 0), 1);
	assert_int_equal(LinceECSCheckMaskBit(record0->archetype->mask, 1), 0);
	assert_int_equal(LinceECSCheckMaskBit(record0->archetype->mask, 2), 0);
	assert_int_equal(record0->archetype->comp_stores.size, 1);
	assert_int_equal(record0->archetype->entity_ids.size, 1);
	assert_int_equal(entity0, *(LinceEntity*)array_get(&record0->archetype->entity_ids,0));

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
	LinceECSComponentStore* store = array_get(&record0->archetype->comp_stores, 0);
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
	// struct Comp1 comp1 = { .x = 5.0, .y = -3.0 };
	// comp0_ptr = LinceECSEmplaceComponent(&ecs, entity0, comp_ids[1], &comp1);
	// assert_non_null(comp0_ptr);

	ecs_debug_print(&ecs);


	// LinceECSAddComponents(&ecs, entity1, 2, (uint32_t[]){comp_ids[0]});

	// Ensure archetype we created before is reused


	/* Remove components */

	LinceECSUninit(&ecs);
}