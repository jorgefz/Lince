
#include "benchmark.h"

#include "lince/containers/linkedlist.h"


void benchmark_array(uint64_t n_iter);
void benchmark_hashmap(uint64_t n_iter);
void benchmark_linkedlist(uint64_t n_iter);
void benchmark_new_ecs(uint64_t n_iter);
void benchmark_old_ecs(uint64_t n_iter);

int main() {
	
#ifndef LINCE_RELEASE
	printf("Please run in Release configuration for more accurate results\n");
#endif

	printf("\n === Array === \n");
	benchmark_array(10000);
	benchmark_array(20000);
	benchmark_array(30000);
	benchmark_array(40000);
	benchmark_array(50000);

	printf("\n === Hahsmap === \n");
	benchmark_hashmap(10000);
	benchmark_hashmap(20000);
	benchmark_hashmap(30000);
	benchmark_hashmap(40000);
	benchmark_hashmap(50000);
	
	/*
	printf("\n === List === \n");
	benchmark_linkedlist(10000);
	benchmark_linkedlist(20000);
	benchmark_linkedlist(30000);
	benchmark_linkedlist(40000);
	benchmark_linkedlist(50000);
	*/

	printf("\n === Old ECS === \n");
	benchmark_old_ecs(10000);
	benchmark_old_ecs(20000);
	benchmark_old_ecs(30000);
	benchmark_old_ecs(40000);
	benchmark_old_ecs(50000);
	// benchmark_old_ecs(75000);
	// benchmark_old_ecs(100000);
	// benchmark_old_ecs(200000);

	printf("\n === New ECS === \n");
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