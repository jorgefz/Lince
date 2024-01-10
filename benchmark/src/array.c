#include "benchmark.h"
#include "lince/containers/array.h"


void benchmark_array(uint64_t n_iter) {

	printf("Array: %llu iterations\n", n_iter);

	array_t a;
	array_init(&a, sizeof(uint64_t));

	/* Insert */
	printf("Insert: ");
	BENCHMARK_LOOP(uint64_t, i, n_iter) {
		array_push_front(&a, &i);
	} BENCHMARK_END(uint64_t, i, n_iter);

	/* Get */
	printf("Get: ");
	BENCHMARK_LOOP(uint64_t, i, n_iter) {
		void* p = array_get(&a, i);
	} BENCHMARK_END(uint64_t, i, n_iter);

	/* Delete */
	printf("Delete: ");
	BENCHMARK_LOOP(uint64_t, i, n_iter) {
		array_pop_front(&a, i);
	} BENCHMARK_END(uint64_t, i, n_iter);

	array_uninit(&a);
}