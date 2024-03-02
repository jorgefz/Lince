#include "lince/utils/uuid.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

void test_uuid(void** state){
    (void)state;
    
    // Test generation
    LinceUUID uuid1 = LinceNewUUID();
    LinceUUID uuid2 = LinceNewUUID();
    assert_memory_not_equal(&uuid1, &uuid2, sizeof(LinceUUID));

    // Test comparison
    LinceUUID uu1 = {.hi = 0x674a713fc074c026, .lo = 0x197a5f6c07a048b9};
    LinceUUID uu2 = {.hi = 0x674a713fc074c026, .lo = 0x197a5f6c07a048b9};
    LinceUUID uu3 = {.lo = 0x674a713fc074c026, .hi = 0x197a5f6c07a048b9};
    assert_true(LinceUUIDEquals(uu1, uu2));
    assert_false(LinceUUIDEquals(uu1, uu3));

    // Test parsing
    LinceUUID uuid = {.hi = 0x674a713fc074c026, .lo = 0x197a5f6c07a048b9};
    const char* result = "26c074c0-3f71-4a67-b948-a0076c5f7a19";
    char buf[LINCE_UUID_STR_SIZE];
    LinceUUIDToStr(uuid, buf, LINCE_UUID_STR_SIZE);
    assert_string_equal(buf, result);
}