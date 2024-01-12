#include "benchmark.h"
#include "lince/containers/hashmap.h"

void benchmark_hashmap(uint64_t n_iter) {

	hashmap_t map;
	hashmap_init(&map, 1);

	/* Set */
	printf("%-10s", "Set: ");
	BENCHMARK_LOOP(uint64_t, i, n_iter) {
		int key = rand();
		hashmap_setb(&map, &key, sizeof(int), &key);
	} BENCHMARK_END(uint64_t, i, n_iter);

	/* Get */
	printf("%-10s", "Get: ");
	BENCHMARK_LOOP(uint64_t, i, n_iter) {
		int key = rand();
		hashmap_getb(&map, &key, sizeof(int));
	} BENCHMARK_END(uint64_t, i, n_iter);

	hashmap_uninit(&map);
	printf("\n");
}