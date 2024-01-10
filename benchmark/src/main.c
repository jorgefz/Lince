
#include "benchmark.h"

#include "lince/containers/linkedlist.h"

void benchmark_linkedlist() {

    int first_value = -1;
    listnode_t* head = list_create(&first_value);

    printf("[BENCHMARK] list_push_back\n");
    BENCHMARK_LOOP(int, i, 50000) {
        list_push_back(head, NULL);
    } BENCHMARK_END(int, i, 50000);

    printf("[BENCHMARK] list_tail\n");
    BENCHMARK_LOOP(int, i, 1) {
        list_node_at(head, i);
    } BENCHMARK_END(int, i, 1);

    list_destroy(head);
}

#include "lince/containers/hashmap.h"

void benchmark_hashmap() {

	int value = 99;
	const int iter = 100000;
	size_t r = 1;

	hashmap_t map;
	hashmap_init(&map, iter);

	for (int i = 0; i != iter; ++i) {
		hashmap_setb(&map, &i, sizeof(int), &value);
	}

	printf("[BENCHMARK] hashmap_getb\n");

	BENCHMARK_LOOP(int, i, iter) {
		r = r && hashmap_getb(&map, &i, sizeof(int));
	} BENCHMARK_END(int, i, iter);

	hashmap_uninit(&map);
	return 0;
}



// void benchmark_hashmap(uint64_t n_iter);
void benchmark_array(uint64_t n_iter);
// void benchmark_linkedlist(uint64_t n_iter);
// void benchmark_old_ecs(uint64_t n_iter);
void benchmark_new_ecs(uint64_t n_iter);
void benchmark_new_ecs(uint64_t n_iter);

int main() {
	
#ifndef LINCE_RELEASE
	printf("Please run in Release configuration for more accurate results\n");
#endif

	benchmark_array(10000);
	benchmark_array(20000);
	benchmark_array(30000);
	benchmark_array(40000);
	benchmark_array(50000);

	return 0;

	// benchmark_old_ecs(10000);
	// benchmark_old_ecs(20000);
	// benchmark_old_ecs(30000);
	// benchmark_old_ecs(40000);
	// benchmark_old_ecs(50000);
	// benchmark_old_ecs(75000);
	// benchmark_old_ecs(100000);
	// benchmark_old_ecs(200000);

	printf("\n\n");

	benchmark_new_ecs(10000);
	benchmark_new_ecs(20000);
	benchmark_new_ecs(30000);
	benchmark_new_ecs(40000);
	benchmark_new_ecs(50000);
	benchmark_new_ecs(75000);
	benchmark_new_ecs(100000);
	benchmark_new_ecs(200000);
	benchmark_new_ecs(300000);
	benchmark_new_ecs(400000);
	benchmark_new_ecs(500000);
	benchmark_new_ecs(750000);
	
	return 0;
}