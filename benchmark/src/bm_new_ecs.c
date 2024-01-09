#include "benchmark.h"
#include "lince/ecs/ecs.h"


uint32_t RandComp() {
	return rand() / (RAND_MAX / (63 + 1) + 1);
}

uint64_t RandMask() {
	return ((uint64_t)rand() << 31) + (uint64_t)rand();
}

void benchmark_new_ecs(uint64_t n_iter) {

	printf(" === NEW ECS BENCHMARK - %llu ITERATIONS ===\n", n_iter);

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
	printf("AddComponents: ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		uint32_t comp_ids[] = { RandComp(), RandComp(), RandComp(), RandComp() };
		LinceECSAddComponents(&ecs, counter, 4, comp_ids);
	} BENCHMARK_END(uint64_t, counter, n_iter);

	/* Get components */
	printf("GetComponent:  ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		LinceECSGetComponent(&ecs, counter, RandComp());
	} BENCHMARK_END(uint64_t, counter, n_iter);

	/* Querying */
	printf("Query:         ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		LinceECSMask mask = { 0 };
		for (uint32_t i = 0; i != 4; ++i) {
			mask[0] &= ((uint64_t)1 << RandComp());
		}
		LinceECSQuery(&ecs, mask, &ecs.query_result);
	} BENCHMARK_END(uint64_t, counter, n_iter);
}