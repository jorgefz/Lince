#include "benchmark.h"
#include "lince/ecs/ecs.h"
#include "lince/entity/entity.h"


uint32_t RandComp() {
	return rand() / (RAND_MAX / (63 + 1) + 1);
}

uint64_t RandMask() {
	return ((uint64_t)rand() << 31) + (uint64_t)rand();
}

void benchmark_new_ecs(uint64_t n_iter) {

	srand((unsigned int)time(NULL));

	LinceECS ecs;
	LinceECSInit(&ecs);

	/* Create components */
	uint32_t comp_size = sizeof(int) * 4;
	for (uint32_t i = 0; i != 64; ++i) {
		LinceECSNewComponent(&ecs, comp_size);
	}

	/* Create entities */
	printf("NewEntity:     ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		LinceECSNewEntity(&ecs);
	} BENCHMARK_END(uint64_t, counter, n_iter);

	/* Add components */
	static uint32_t prefab_arch_count = 10;
	uint32_t comp_ids[][4] = {
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() }
	};

	printf("AddComponents: ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		LinceECSAddComponents(&ecs, counter, 4, comp_ids[rand() % prefab_arch_count]);
	} BENCHMARK_END(uint64_t, counter, n_iter);

	/* Get components */
	printf("GetComponent:  ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		LinceECSGetComponent(&ecs, counter, RandComp());
	} BENCHMARK_END(uint64_t, counter, n_iter);

	/* Querying */
	printf("Query:         ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		LinceECSQuery(&ecs, &ecs.query_result, 4, (uint32_t[]){RandComp(), RandComp(), RandComp(), RandComp()});
		array_clear(&ecs.query_result);
	} BENCHMARK_END(uint64_t, counter, n_iter);

	printf("\n");
}


void benchmark_old_ecs(uint64_t n_iter) {

	srand((uint32_t)time(NULL));

	uint32_t s = (uint32_t)(sizeof(int) * 4);
	LinceEntityRegistry* reg;

	/* Create components */
	reg = LinceCreateEntityRegistry(64,
		s, s, s, s, s, s, s, s, s, s, s, s, s, s, s, s,
		s, s, s, s, s, s, s, s, s, s, s, s, s, s, s, s,
		s, s, s, s, s, s, s, s, s, s, s, s, s, s, s, s,
		s, s, s, s, s, s, s, s, s, s, s, s, s, s, s, s
	);

	/* Create entities */
	printf("NewEntity:     ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		LinceCreateEntity(reg);
	} BENCHMARK_END(uint64_t, counter, n_iter);

	/* Add components */
	static uint32_t prefab_arch_count = 10;
	uint32_t comp_ids[][4] = {
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() },
		{ RandComp(), RandComp(), RandComp(), RandComp() }
	};

	printf("AddComponents: ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		uint32_t idx = rand() % 4;
		for (uint32_t i = 0; i != 4; ++i) {
			LinceAddEntityComponent(reg, (uint32_t)counter, comp_ids[idx][i], (int[]) { 1, 2, 3, 4 });
		}
	} BENCHMARK_END(uint64_t, counter, n_iter);

	/* Get components */
	printf("GetComponent:  ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		LinceGetEntityComponent(reg, (uint32_t)counter, RandComp());
	} BENCHMARK_END(uint64_t, counter, n_iter);

	/* Querying */
	printf("Query:         ");
	array_t query;
	array_init(&query, sizeof(uint32_t));
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		LinceQueryEntities(reg, &query, 4, RandComp(), RandComp(), RandComp(), RandComp());
		array_clear(&query);
	} BENCHMARK_END(uint64_t, counter, n_iter);
	array_uninit(&query);

	printf("\n");
}