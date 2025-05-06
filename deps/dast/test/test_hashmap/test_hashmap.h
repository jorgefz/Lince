#ifndef TEST_HASHMAP_H
#define TEST_HASHMAP_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "hashmap.h"


#define TEST_GROUP_HASHMAP \
    cmocka_unit_test(test_hashmap_init_free), \
    cmocka_unit_test(test_hashmap_init_null), \
    cmocka_unit_test(test_hashmap_init_zero_size), \
    cmocka_unit_test(test_hashmap_init_custom_alloc), \
    cmocka_unit_test(test_hashmap_init_custom_hash), \
    cmocka_unit_test(test_hashmap_init_custom_cmp), \
    cmocka_unit_test(test_hashmap_setb), \
    cmocka_unit_test(test_hashmap_setb_null_value), \
    cmocka_unit_test(test_hashmap_setb_bad_key), \
    cmocka_unit_test(test_hashmap_getb_none), \
    cmocka_unit_test(test_hashmap_has_keyb), \
    cmocka_unit_test(test_hashmap_has_keyb_false), \
    cmocka_unit_test(test_hashmap_iterb), \
    cmocka_unit_test(test_hashmap_iterb_empty), \
    cmocka_unit_test(test_hashmap_set_str), \
    cmocka_unit_test(test_hashmap_has_key_str), \
    cmocka_unit_test(test_hashmap_iter_str), 
    


void test_hashmap_init_free(void** state);
void test_hashmap_init_null(void** state);
void test_hashmap_init_zero_size(void** state);
void test_hashmap_init_custom_alloc(void** state);
void test_hashmap_init_custom_hash(void** state);
void test_hashmap_init_custom_cmp(void** state);
void test_hashmap_setb(void** state);
void test_hashmap_setb_null_value(void** state);
void test_hashmap_setb_bad_key(void** state);
void test_hashmap_getb_none(void** state);
void test_hashmap_has_keyb(void** state);
void test_hashmap_has_keyb_false(void** state);
void test_hashmap_iterb(void** state);
void test_hashmap_iterb_empty(void** state);
void test_hashmap_set_str(void** state);
void test_hashmap_has_key_str(void** state);
void test_hashmap_iter_str(void** state);


#endif /* TEST_HASHMAP_H */
