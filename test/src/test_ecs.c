#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <lince/ecs/ecs.h>
#include "benchmark.h"


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
	LinceECS ecs, *r;
	r = LinceECSInit(&ecs);

	assert_non_null(r);
	assert_int_equal(&ecs, r);
	assert_int_equal(ecs.entity_records.size, 0);
	assert_int_equal(ecs.component_sizes.size, 0);
	assert_int_equal(ecs.archetypes.size, 0);
	assert_int_equal(ecs.component_index.size, 0);
	assert_int_equal(ecs.entity_pool.size, 0);
	assert_non_null(ecs.archetype_map.table);

	/* --- Creating components --- */
	uint32_t comp_sizes[] = { 8, 16, 32 };
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
	uint32_t col0 = (uint64_t)hashmap_getb(comp_arch0, arch_mask, sizeof(LinceECSMask));
	assert_int_equal(col0, col_comp0); // index of comp store in archetype

	hashmap_t* comp_arch2 = array_get(&ecs.component_index, comp_ids[2]);
	assert_non_null(comp_arch2);
	uint32_t col2 = (uint64_t)hashmap_getb(comp_arch2, arch_mask, sizeof(LinceECSMask));
	assert_int_equal(col2, col_comp2); // index of comp store in archetype

	/* --- Creating entities --- */
	LinceEntity entity = LinceECSNewEntity(&ecs);

	assert_int_equal(entity, 0);
	assert_int_equal(ecs.entity_count, 1);
	LinceECSRecord* record = array_get(&ecs.entity_records, (uint32_t)entity);
	assert_non_null(record);
	assert_int_equal(record->row, 0);
	assert_memory_equal(record->mask, (LinceECSMask) { 0 }, sizeof(LinceECSMask));
	assert_null(record->archetype);

	/* Add components */

	/* Remove components */

	LinceECSUninit(&ecs);
}