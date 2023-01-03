
#include "test.h"


void run_all_tests(void);


/* CONTAINERS */
void batch_test_containers();
int test_array();
int test_hashmap();
int test_linkedlist();

int test_hashmap_performance();
int test_linkedlist_performance();
int test_array_performance();

/* ECS */
void batch_test_entity();
int test_entity();
int test_entity_performance();
