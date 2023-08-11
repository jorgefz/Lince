#include "test.h"


#define TEST_PASSED_STR "["GREEN"PASSED"NOCOLOR"]"
#define TEST_FAILED_STR "["RED"FAILED"NOCOLOR"]"
#define GET_TEST_STR(result) ( (result == TEST_PASS) ? TEST_PASSED_STR : TEST_FAILED_STR )

void run_tests(struct test_t* tests, uint32_t count, const char* batch_name){
	
	printf("\n[%s] \n", batch_name);
	
	for(struct test_t* t = tests; t != tests + count; ++t){
		int result = t->fn();
		printf("%-40s%s\n", t->name, GET_TEST_STR(result));
	}

}