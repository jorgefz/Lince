	#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/containers/array.h"


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
	assert_null(x.begin);
	assert_null(x.end);

	array_uninit(&x);
	assert_true(x.size==0);
	assert_true(x.capacity==0);
	assert_null(x.data);
	assert_true(x.element_size==0);
	assert_null(x.begin);
	assert_null(x.end);
	
	// Initialise with allocated memory
	array_t* a = array_create(sizeof(int));
	assert_non_null(a);
	assert_true(a->element_size==sizeof(int));
	assert_true(a->size==0);
	assert_null(a->begin);
	assert_null(a->end);

	// Resize
	r = array_resize(a, 10);
	assert_non_null(r);
	assert_true(a->size==10);
	assert_true(a->capacity==16);
	assert_non_null(a->data);
	assert_ptr_equal(a->begin, a->data);
	assert_ptr_equal(a->end, (char*)a->data + sizeof(int)*10);

	// Setting elements
	int res = 1;
	for(int i = 0; i != (int)a->size; ++i){
		res = res && array_set(a, &i, i);
	}
	assert_true(res);

	// Getting elements with indices
	res = 1;
	for(int i = 0; i != (int)a->size; ++i){
		res = res && (*(int*)array_get(a, i) == i);
	}
	assert_true(res);

	// Getting elements with iterator
	res = 1;
	int counter = 0;
	for(int* item = a->begin; item!=a->end; ++item){
		res = res && (*item == counter++);
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
	assert_ptr_equal(a->begin, a->data);
	assert_ptr_equal(a->end, (char*)a->data + sizeof(int)*a->size);

	// Removing a value
	r = array_remove(a, 1);
	r = array_pop_back(a);
	r = array_pop_front(a);

	res = r && (a->size == 8)
			&& (*(int*)array_front(a)==2)
			&& (*(int*)array_back(a)==8);
	assert_true(res);
	assert_ptr_equal(a->begin, a->data);
	assert_ptr_equal(a->end, (char*)a->data + sizeof(int)*a->size);

	// array_clear
	r = array_clear(a);
	assert_non_null(r);
	assert_true(a->size==0);
	assert_ptr_equal(a->begin, a->data);
	assert_ptr_equal(a->end, a->data);

	array_destroy(a);

}
