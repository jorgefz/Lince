
#ifndef LINCE_TEST_H
#define LINCE_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

typedef int (*test_fn_t)(void);

enum {TEST_PASS, TEST_FAIL};

#define TEST_ASSERT(x, msg) if(!(x)){ printf("%s, line %d - %s\n",__FILE__,__LINE__,(msg)); return TEST_FAIL; }

#define TEST_CLOCK_START(time_name) clock_t time_name = clock()

#define TEST_CLOCK_END(time_name, opnum) do{ 						\
	time_name = clock() - time_name; 								\
	printf("%s: time taken: %ld ms for %ld ops (%f ops/ms)\n", 		\
		__FUNCTION__, time_name * 1000 / CLOCKS_PER_SEC, opnum,		\
		(float)opnum/(float)(time_name * 1000 / CLOCKS_PER_SEC)); 	\
} while(0) 															\

struct test_t {
	test_fn_t fn;
	char name[100];
	//int passed;
};


/* ANSI colour codes */
#define GREEN "\e[1;32m"
#define RED "\e[1;31m"
#define NOCOLOR "\e[0m"


#endif /* LINCE_TEST_H */