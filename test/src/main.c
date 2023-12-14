
// #include "test.h"
// #include "targets.h"
#define UNIT_TESTING 1
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

void test_array(void** state);
void test_hashmap(void** state);
void test_hashmap_byte_key(void** state);
void test_linkedlist(void** state);
void test_entity(void** state);
void test_uuid(void** state);
void test_point(void** state);
void test_transform(void** state);
void test_box2d(void** state);

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_array),
        cmocka_unit_test(test_hashmap),
        cmocka_unit_test(test_hashmap_byte_key),
        cmocka_unit_test(test_linkedlist),
        cmocka_unit_test(test_entity),
        cmocka_unit_test(test_uuid),
        cmocka_unit_test(test_point),
        cmocka_unit_test(test_transform),
        cmocka_unit_test(test_box2d)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}