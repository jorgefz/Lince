#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#define LINCE_ECS_MAX_COMPONENTS_FACTOR 4

#include <lince/ecs/ecs.h>
#include "benchmark.h"

/* The static functions below were copied from 'ecs.c' */

/** @brief Sets the bit corresponding to a component ID on a bitmask */
static void LinceECSSetMaskBit(LinceECSMask mask, uint32_t comp_id) {
	mask[comp_id / 64] |= ((uint64_t)1 << (comp_id % 64));
}

/** @brief Unsets the bit corresponding to a component ID on a bitmask */
static void LinceECSUnsetMaskBit(LinceECSMask mask, uint32_t comp_id) {
	mask[comp_id / 64] &= ~((uint64_t)1 << (comp_id % 64));
}

/** @brief Returns true if a given bit is set on a bit mask */
static LinceBool LinceECSCheckMaskBit(LinceECSMask mask, uint32_t comp_id) {
	return !!(mask[comp_id / 64] & ((uint64_t)1 << (comp_id % 64)));
}

void test_ecs_mask(void** state) {

	assert_int_equal(LINCE_ECS_MAX_COMPONENTS_FACTOR, 4);
	assert_int_equal(sizeof(LinceECSMask), sizeof(uint64_t) * 4);

	LinceECSMask mask = { 0, 0, 0, 0 };

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
	LinceECSSetMaskBit(mask, 200);
	assert_int_equal(mask[0], ((uint64_t)1 << 63) | ((uint64_t)1 << 25));
	assert_int_equal(mask[3], (uint64_t)1 << 8);
	assert_int_equal(LinceECSCheckMaskBit(mask, 25), 1);
	assert_int_equal(LinceECSCheckMaskBit(mask, 200), 1);
}


void test_ecs(void** state) {

	/* Initialisation */
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

	/* Creating Components */
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

	/* Creating archetypes */
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

	assert_ptr_equal(arch, hashmap_get(&ecs.archetype_map, arch_mask, sizeof(LinceECSMask)));

	// Check component index

	// Create entity

	// Add components and check that archetypes are created at every step

	LinceECSUninit(&ecs);
}