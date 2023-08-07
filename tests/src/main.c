
// #include "test.h"
// #include "targets.h"
#define UNIT_TESTING 1
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "test_array.h"

/* A test case that does nothing and succeeds. */
static void null_test_success(void **state) {
    (void) state; /* unused */
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test_success),
        cmocka_unit_test(test_array),
    };
 
    return cmocka_run_group_tests(tests, NULL, NULL);
}