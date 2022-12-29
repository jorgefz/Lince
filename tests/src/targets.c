
#include "targets.h"

void run_all_tests(void){
    batch_test_containers();
}

void batch_test_containers(){
	struct test_t tests[] = {
		{.fn = test_array,          .name = "test_array"},
		{.fn = test_hashmap,        .name = "test_hashmap"},
        {.fn = test_linkedlist,     .name = "test_linkedlist"},

		{.fn = test_array_performance, .name = "test_array_performance"},
		{.fn = test_hashmap_performance, .name = "test_hashmap_performance"},
		{.fn = test_linkedlist_performance, .name = "test_linkedlist_performance"},
	};
	uint32_t count = sizeof(tests) / sizeof(struct test_t);
	run_tests(tests, count, "containers");
}
