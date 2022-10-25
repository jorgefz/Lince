#include "test.h"

void run_tests(struct test_t* tests, uint32_t count, const char* batch_name){
	
	printf("\n");
	printf("[%s]\n", batch_name);
	for(uint32_t i = 0; i != count; ++i){
		struct test_t* t = tests + i;
		if(t->fn() == TEST_PASS) printf("["GREEN"PASSED"NOCOLOR"]");
		else printf("["RED"FAILED"NOCOLOR"]");
		printf(" %s\n", t->name);
	}
}