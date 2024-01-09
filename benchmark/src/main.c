
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


#include "lince/containers/array.h"

void benchmark_array() {

	array_t nums;
	array_init(&nums, sizeof(int));

	BENCHMARK_LOOP(int, i, 10000000) {
		array_push_back(&nums, &i);
	} BENCHMARK_END(int, i, 10000000);

	array_uninit(&nums);
	return 0;
}

#include "lince/entity/entity.h"

uint32_t RandComp();

void benchmark_old_ecs(uint32_t n_iter) {
	
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
	printf("AddComponents: ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		for (uint32_t i = 0; i != 4; ++i) {
			LinceAddEntityComponent(reg, (uint32_t)counter, RandComp(), (int[]) { 1, 2, 3, 4 });
		}
	} BENCHMARK_END(uint64_t, counter, n_iter);

	/* Get components */
	printf("GetComponent:  ");
	BENCHMARK_LOOP(uint64_t, counter, n_iter) {
		LinceGetEntityComponent(reg, counter, RandComp());
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
}

// void benchmark_hashmap(uint64_t n_iter);
// void benchmark_array(uint64_t n_iter);
// void benchmark_linkedlist(uint64_t n_iter);
// void benchmark_old_ecs(uint64_t n_iter);
void benchmark_new_ecs(uint64_t n_iter);

int main() {
	
#ifndef LINCE_RELEASE
	printf("Please run in Release configuration\n");
	// return 0;
#endif

	benchmark_old_ecs(20000);
	benchmark_new_ecs(20000);
	
	return 0;

	benchmark_new_ecs(1000);
	benchmark_new_ecs(2500);
	benchmark_new_ecs(5000);
	benchmark_new_ecs(7500);

	benchmark_new_ecs(10000);
	benchmark_new_ecs(15000);
	benchmark_new_ecs(20000);
	benchmark_new_ecs(25000);
	benchmark_new_ecs(30000);
	benchmark_new_ecs(40000);
	benchmark_new_ecs(50000);

	return 0;
}