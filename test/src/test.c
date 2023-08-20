#include "test.h"


#define TEST_PASSED_STR "["GREEN"PASSED"NOCOLOR"]"
#define TEST_FAILED_STR "["RED"FAILED"NOCOLOR"]"
#define GET_TEST_STR(result) ( (result == TEST_PASS) ? TEST_PASSED_STR : TEST_FAILED_STR )
