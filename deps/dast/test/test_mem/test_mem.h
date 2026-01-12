#ifndef TEST_MEM
#define TEST_MEM

#define TEST_GROUP_MEM \
    cmocka_unit_test(test_default_alloc), \
    cmocka_unit_test(test_get_set_alloc), \
    cmocka_unit_test(test_fallback_alloc), \
    cmocka_unit_test(test_memeq), \
    cmocka_unit_test(test_memset), \
    cmocka_unit_test(test_memcpy), \
    cmocka_unit_test(test_memmove)

void test_default_alloc(void**);
void test_get_set_alloc(void**);
void test_fallback_alloc(void**);
void test_memeq(void**);
void test_memset(void**);
void test_memcpy(void**);
void test_memmove(void**);

#endif /* TEST_MEM */