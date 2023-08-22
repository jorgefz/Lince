
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
void test_linkedlist(void** state);
void test_entity(void** state);
void test_uuid(void** state);

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_array),
        cmocka_unit_test(test_hashmap),
        cmocka_unit_test(test_linkedlist),
        cmocka_unit_test(test_entity),
        cmocka_unit_test(test_uuid)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}