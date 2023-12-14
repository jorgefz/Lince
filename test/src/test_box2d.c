#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/physics/box2d.h"


void test_box2d(void** state) {
	(void)state;

	LinceBox2D a = { .x = 0.5, .y = 0.5, .w = 1, .h = 1 };
	assert_true(LinceBox2DCollides(&a, &a));

	LinceBox2D b = { .x = 2, .y = 0.5, .w = 1, .h = 1 };
	assert_false(LinceBox2DCollides(&a, &b));

	LinceBox2D c = { .x = 1, .y = 0.5, .w = 1, .h = 1 };
	assert_true(LinceBox2DCollides(&a, &c));

	LinceBox2D d = { .x = 1.5, .y = 0.5, .w = 1, .h = 1 };
	assert_true(LinceBox2DCollides(&a, &d));

	LinceBox2D e = { .x = 1.5, .y = 1.5, .w = 1, .h = 1 };
	assert_true(LinceBox2DCollides(&a, &e));

	LinceBox2D f = { .x = 1.5, .y = 1.5+1e-6, .w = 1, .h = 1 };
	assert_false(LinceBox2DCollides(&a, &f));
}