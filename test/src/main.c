
#include "lince.h"

// #include "test.h"
// #include "targets.h"
#define UNIT_TESTING 1
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>


// #include "test_asset_cache/test_asset_cache.h"


void test_uuid_gen(void** state);
void test_uuid_comp(void** state);
void test_uuid_to_str(void** state);
void test_uuid_from_str(void** state);

#define TEST_GROUP_UUID \
    cmocka_unit_test(test_uuid_gen), \
    cmocka_unit_test(test_uuid_comp), \
    cmocka_unit_test(test_uuid_to_str), \
    cmocka_unit_test(test_uuid_from_str)

void test_linkedlist(void** state);
void test_entity(void** state);
void test_point(void** state);
void test_transform(void** state);
void test_box2d(void** state);
void test_ecs_mask(void** state);
void test_ecs_system(void** state);
void test_ecs(void** state);


int main() {

    LinceLoggerDefaultToStderr(0);
    // LinceAllocatorInit();

    static const struct CMUnitTest tests[] = {
        // TEST_GROUP_ASSET_CACHE,
        TEST_GROUP_UUID,
        cmocka_unit_test(test_entity),
        cmocka_unit_test(test_point),
        cmocka_unit_test(test_transform),
        cmocka_unit_test(test_box2d),
        cmocka_unit_test(test_ecs_mask),
        cmocka_unit_test(test_ecs),
        cmocka_unit_test(test_ecs_system),
        
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}