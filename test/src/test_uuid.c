#include "lince/utils/uuid.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>


void test_uuid_gen(void** state){
    (void)state;
    LinceUUID uuid1 = LinceNewUUID();
    LinceUUID uuid2 = LinceNewUUID();
    assert_memory_not_equal(&uuid1, &uuid2, sizeof(LinceUUID));
}

void test_uuid_comp(void** state){
    (void)state;
    LinceUUID uu1 = {.hi = 0x674a713fc074c026, .lo = 0x197a5f6c07a048b9};
    LinceUUID uu2 = {.hi = 0x674a713fc074c026, .lo = 0x197a5f6c07a048b9};
    LinceUUID uu3 = {.lo = 0x674a713fc074c026, .hi = 0x197a5f6c07a048b9};
    assert_true(LinceUUIDEquals(uu1, uu2));
    assert_false(LinceUUIDEquals(uu1, uu3));
}

void test_uuid_to_str(void** state){
    (void)state;
    LinceUUID uuid = {.hi = 0x674a713fc074c026, .lo = 0x197a5f6c07a048b9};
    string_t str = string_scoped_lit("26c074c0-3f71-4a67-b948-a0076c5f7a19");
    string_t result = LinceUUIDToStr(uuid);
    assert_string_equal(str.str, result.str);
}

void test_uuid_from_str(void** state){
    (void)state;
    LinceUUID uuid = {.hi = 0x674a713fc074c026, .lo = 0x197a5f6c07a048b9};
    string_t str = string_scoped_lit("26c074c0-3f71-4a67-b948-a0076c5f7a19");
    LinceUUID result = LinceUUIDFromStr(str);
    assert_memory_equal(&uuid, &result, sizeof(LinceUUID));
}