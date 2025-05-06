#ifndef TEST_ARCH
#define TEST_ARCH

#define TEST_GROUP_ARCH \
    cmocka_unit_test(test_arch), \
    cmocka_unit_test(test_i8),   \
    cmocka_unit_test(test_u8),   \
    cmocka_unit_test(test_i16),  \
    cmocka_unit_test(test_u16),  \
    cmocka_unit_test(test_i32),  \
    cmocka_unit_test(test_u32),  \
    cmocka_unit_test(test_i64),  \
    cmocka_unit_test(test_u64),  \
    cmocka_unit_test(test_bool), \
    cmocka_unit_test(test_sz)

void test_arch(void**);
void test_i8(void**);
void test_u8(void**);
void test_i16(void**);
void test_u16(void**);
void test_i32(void**);
void test_u32(void**);
void test_i64(void**);
void test_u64(void**);
void test_bool(void**);
void test_sz(void**);



#endif /* TEST_ARCH */