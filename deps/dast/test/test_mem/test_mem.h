#ifndef TEST_MEM
#define TEST_MEM

#define TEST_GROUP_MEM \
    cmocka_unit_test(test_memeq), \
    cmocka_unit_test(test_memset), \
    cmocka_unit_test(test_memcpy), \
    cmocka_unit_test(test_memmove)

void test_memeq(void**);
void test_memset(void**);
void test_memcpy(void**);
void test_memmove(void**);

#endif /* TEST_MEM */