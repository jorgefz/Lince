#ifndef TEST_STRING_H
#define TEST_STRING_H

#define TEST_GROUP_STRING \
    cmocka_unit_test(test_string_scoped), \
    cmocka_unit_test(test_string_scoped_lit), \
    cmocka_unit_test(test_string_default_alloc), \
    cmocka_unit_test(test_string_get_alloc), \
    cmocka_unit_test(test_string_from_len), \
    cmocka_unit_test(test_string_from_len_zero), \
    cmocka_unit_test(test_string_from_chars), \
    cmocka_unit_test(test_string_from_chars_null), \
    cmocka_unit_test(test_string_from_chars_null), \
    cmocka_unit_test(test_string_from_literal), \
    cmocka_unit_test(test_string_from_literal_empty), \
    cmocka_unit_test(test_string_from_fmt), \
    cmocka_unit_test(test_string_copy_custom), \
    cmocka_unit_test(test_string_copy)


void test_string_scoped(void** state);
void test_string_scoped_lit(void** state);
void test_string_default_alloc(void** state);
void test_string_get_alloc(void** state);
void test_string_from_len(void** state);
void test_string_from_len_zero(void** state);
void test_string_from_chars(void** state);
void test_string_from_chars_null(void** state);
void test_string_free(void** state);
void test_string_from_literal(void** state);
void test_string_from_literal_empty(void** state);
void test_string_from_fmt(void** state);
void test_string_copy_custom(void** state);
void test_string_copy(void** state);

#endif /* TEST_STRING_H */