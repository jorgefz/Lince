#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef int (*test_fn_t)(void);

enum {TEST_PASS, TEST_FAIL};

struct test_t {
	test_fn_t fn;
	char name[100];
	int passed;
};

void run_tests(struct test_t* tests, uint32_t count, const char* batch_name);

/* ANSI colour codes */
#define GREEN "\e[1;32m"
#define RED "\e[1;31m"
#define NOCOLOR "\e[0m"
