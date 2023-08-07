#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/containers/array.h"

#include "test.h"
#include "targets.h"


static void scan_array(array_t* a){
	if(!a) return;
	printf(" array of size %u, capacity %u, and element size %u\n",
		a->size, a->capacity, a->element_size);
	for(uint32_t i = 0; i != a->size; ++i){
		char* p = (char*)a->data + i * a->element_size;
		printf("[%u] %d at %p\n", i, *(int*)p, p);
	}
}

static int test_array_performance(){

	array_t nums;
	array_init(&nums, sizeof(int));

	TEST_CLOCK_START(time);
    int end = 10000000;
	for(int i = 0; i != end; ++i){
        array_push_back(&nums, &i);
	}
	long int n_op = end;
	TEST_CLOCK_END(time, n_op);
	
    array_uninit(&nums);
	return TEST_PASS;
}



void test_array(void** state){
	(void)state;

	array_t *r = NULL;

	// Create with element size 0
	// Should fail all attempts to insert or manipulate via provided functions
	array_t z;
	array_init(&z, 0);
	r = array_resize(&z, 10);
	// TEST_ASSERT(!r, "Array of element size zero was able to be resized");
	assert_null(r);

	r = array_insert(&z, (void*)5, 0);
	// TEST_ASSERT(!r, "Array of element size zero could insert an element");
	assert_null(r);

	array_uninit(&z);

	// Create with ints
	array_t x;
	array_init(&x, sizeof(int));
	// TEST_ASSERT((x.size==0) && (x.capacity==0) && (!x.data),
	// 	"Failed to init array with elements of size int");
	assert_true(x.size==0);
	assert_true(x.capacity==0);
	assert_null(x.data);

	array_uninit(&x);
	//TEST_ASSERT(x.size==0 && x.capacity==0 && x.data==NULL && x.element_size==0,
	//	"Failed to uninit array");
	assert_true(x.size==0);
	assert_true(x.capacity==0);
	assert_null(x.data);
	assert_true(x.element_size==0);
	
	// Create using allocator
	array_t* a = array_create(sizeof(int));
	// TEST_ASSERT(a && a->element_size==sizeof(int) && a->size==0,
	// 	"Failed to allocate array with elements of size int");
	assert_non_null(a);
	assert_true(a->element_size==sizeof(int));
	assert_true(a->size==0);

	// Resize
	r = array_resize(a, 10);
	// TEST_ASSERT(r, "Failed to resize array of ints from size 0 to 10");
	// TEST_ASSERT((a->size==10) && (a->capacity==16) && (a->data),
	// 	"Failed to resize array of ints from size 0 to 10");
	assert_non_null(r);
	assert_true(a->size==10);
	assert_true(a->capacity==16);
	assert_non_null(a->data);

	// Setting elements
	int res = 1;
	for(int i = 0; i != (int)a->size; ++i){
		res = res && array_set(a, &i, i);
	}
	// TEST_ASSERT(res, "Failed to set array elements incrementally from 0 to 10");
	assert_true(res);

	// Getting elements
	res = 1;
	for(int i = 0; i != (int)a->size; ++i){
		res = res && (*(int*)array_get(a, i) == i);
	}
	// TEST_ASSERT(res, "Failed to retrieve items, or retrieved item is wrong")
	assert_true(res);

	res = (*(int*)array_front(a)==0) && (*(int*)array_back(a)==9);
	// TEST_ASSERT(res, "Wrong front or back value");
	assert_true(res);

	// Case: value passed is NULL
	r = array_set(a, NULL, 1);
	res = (r) && (*(int*)array_get(a,1) == 0);
	// TEST_ASSERT(res, "Set value to NULL does not zero the value");
	assert_true(res);

	// Limiting case: index out of bounds
	r = array_set(a, NULL, a->size);
	// TEST_ASSERT(!r, "Setting out-of-bounds value does not return NULL");
	assert_null(r);

	// Inserting a value
	int prev = *(int*)array_get(a, 5);
	int value = 99;
	r = array_insert(a, &value, 5);

	res = r && (a->size == 11);
	res = res && (*(int*)array_get(a,5)==value) && (*(int*)array_get(a,6)==prev);
	// TEST_ASSERT(res, "Failed to insert value");
	assert_true(res);

	// Removing a value
	r = array_remove(a, 1);
	r = array_pop_back(a);
	r = array_pop_front(a);

	res = r && (a->size == 8)
			&& (*(int*)array_front(a)==2)
			&& (*(int*)array_back(a)==8);
	// TEST_ASSERT(res, "Failed to pop first or last value");
	assert_true(res);

	// array_clear
	r = array_clear(a);
	// TEST_ASSERT(r && a->size==0, "Failed to clear array");
	assert_non_null(r);
	assert_true(a->size==0);

	array_destroy(a);

}
