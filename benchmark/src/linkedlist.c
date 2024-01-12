#include "benchmark.h"
#include "lince/containers/linkedlist.h"

void benchmark_linkedlist(uint64_t n_iter) {

	listnode_t* list = list_create(NULL);
	list_push_back(list, NULL);

	/* Insert */
	printf("Insert:  ");
	BENCHMARK_LOOP(uint64_t, i, n_iter) {
		list_insert_at(list, rand() % (i+1), NULL);
	} BENCHMARK_END(uint64_t, i, n_iter);

	/* Get */
	printf("Get:     ");
	uint32_t size = list_size(list);
	BENCHMARK_LOOP(uint64_t, i, n_iter) {
		list_node_at(list, rand() % size);
	} BENCHMARK_END(uint64_t, i, n_iter);

	/* Delete */
	list_destroy(list);
	
}