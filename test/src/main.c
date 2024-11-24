
// #include "test.h"
// #include "targets.h"
#define UNIT_TESTING 1
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/core/logger.h"
#include "test_asset_cache/test_asset_cache.h"
#include "test_array/test_array.h"
#include "test_string/test_string.h"
#include "test_hashmap/test_hashmap.h"

void test_linkedlist(void** state);
void test_entity(void** state);
void test_uuid(void** state);
void test_point(void** state);
void test_transform(void** state);
void test_box2d(void** state);
void test_ecs_mask(void** state);
void test_ecs_system(void** state);
void test_ecs(void** state);


int main() {

    LinceLoggerDefaultToStderr(0);

    static const struct CMUnitTest tests[] = {
        TEST_GROUP_ARRAY,
        TEST_GROUP_STRING,
        TEST_GROUP_ASSET_CACHE,
        TEST_GROUP_HASHMAP,

        cmocka_unit_test(test_linkedlist),
        cmocka_unit_test(test_entity),
        cmocka_unit_test(test_uuid),
        cmocka_unit_test(test_point),
        cmocka_unit_test(test_transform),
        cmocka_unit_test(test_box2d),
        cmocka_unit_test(test_ecs_mask),
        cmocka_unit_test(test_ecs),
        cmocka_unit_test(test_ecs_system),
        
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}