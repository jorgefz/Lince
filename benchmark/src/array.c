#include "benchmark.h"
#include "lince/containers/array.h"


void benchmark_array(uint64_t n_iter) {

	array_t a;
	array_init(&a, sizeof(uint64_t));

	/* Insert */
	printf("%-10s", "Insert: ");
	BENCHMARK_LOOP(uint32_t, i, n_iter) {
		array_push_front(&a, (uint32_t[]){rand()});
	} BENCHMARK_END(uint64_t, i, n_iter);

	/* Get */
	printf("%-10s", "Get: ");
	BENCHMARK_LOOP(uint64_t, i, n_iter) {
		void* p = array_get(&a, rand() % a.size);
	} BENCHMARK_END(uint64_t, i, n_iter);

	/* Delete */
	printf("%-10s", "Delete: ");
	BENCHMARK_LOOP(uint64_t, i, n_iter) {
		array_pop_front(&a);
	} BENCHMARK_END(uint64_t, i, n_iter);

	array_uninit(&a);
	printf("\n");
}