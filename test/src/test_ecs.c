#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#define LINCE_ECS_MAX_COMPONENTS_FACTOR 4

#include <lince/ecs/ecs.h>
#include "benchmark.h"

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
	// assert_int_equal(mask[3], ((uint64_t)1 << 56));
	assert_int_equal(LinceECSCheckMaskBit(mask, 25), 1);
	assert_int_equal(LinceECSCheckMaskBit(mask, 200), 1);
}
