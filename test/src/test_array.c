#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/containers/array.h"

#include "test.h"


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

	// Initialise array with element size 0
	// Functions should return NULL value
	array_t z;
	array_init(&z, 0);
	r = array_resize(&z, 10);
	assert_null(r);
	r = array_insert(&z, (void*)5, 0);
	assert_null(r);
	array_uninit(&z);

	// Initialise array of integers
	array_t x;
	array_init(&x, sizeof(int));
	assert_true(x.size==0);
	assert_true(x.capacity==0);
	assert_null(x.data);

	array_uninit(&x);
	assert_true(x.size==0);
	assert_true(x.capacity==0);
	assert_null(x.data);
	assert_true(x.element_size==0);
	
	// Initialise with allocated memory
	array_t* a = array_create(sizeof(int));
	assert_non_null(a);
	assert_true(a->element_size==sizeof(int));
	assert_true(a->size==0);

	// Resize
	r = array_resize(a, 10);
	assert_non_null(r);
	assert_true(a->size==10);
	assert_true(a->capacity==16);
	assert_non_null(a->data);

	// Setting elements
	int res = 1;
	for(int i = 0; i != (int)a->size; ++i){
		res = res && array_set(a, &i, i);
	}
	assert_true(res);

	// Getting elements
	res = 1;
	for(int i = 0; i != (int)a->size; ++i){
		res = res && (*(int*)array_get(a, i) == i);
	}
	assert_true(res);

	res = (*(int*)array_front(a)==0) && (*(int*)array_back(a)==9);
	assert_true(res);

	// Case: value passed is NULL
	r = array_set(a, NULL, 1);
	res = (r) && (*(int*)array_get(a,1) == 0);
	assert_true(res);

	// Limiting case: index out of bounds
	r = array_set(a, NULL, a->size);
	assert_null(r);

	// Inserting a value
	int prev = *(int*)array_get(a, 5);
	int value = 99;
	r = array_insert(a, &value, 5);

	res = r && (a->size == 11);
	res = res && (*(int*)array_get(a,5)==value) && (*(int*)array_get(a,6)==prev);
	assert_true(res);

	// Removing a value
	r = array_remove(a, 1);
	r = array_pop_back(a);
	r = array_pop_front(a);

	res = r && (a->size == 8)
			&& (*(int*)array_front(a)==2)
			&& (*(int*)array_back(a)==8);
	assert_true(res);

	// array_clear
	r = array_clear(a);
	assert_non_null(r);
	assert_true(a->size==0);

	array_destroy(a);

}
