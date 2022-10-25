#include "tests.h"
#include "test.h"
#include "lince/containers/array.h"

int test_array_create_destroy(){
	struct array_t* a;
	int cond;

	a = array_create(sizeof(int));
	cond = (a) && (a->size==0) && (a->capacity==0) && (!a->data);
	if(!cond) return TEST_FAIL;
	array_destroy(a);

	a = array_create(0);
	cond = (!a);
	if(!cond) {
		array_destroy(a);
		return TEST_FAIL;
	}

	return TEST_PASS;
}


int test_array_resize(){

	struct array_t *a, *r;
	int cond;

	a = array_create(sizeof(int));
	r = array_resize(a, 10);
	cond = a && r && (a->size==10) && (a->capacity==16) && (a->data);
	if(!cond) return TEST_FAIL;

	return TEST_PASS;
}

int test_array_set_get(){
	struct array_t *a, *r;
	int cond;

	a = array_create(sizeof(int));
	r = array_resize(a, 10);
	cond = a && r && (a->size==10) && (a->capacity==16) && (a->data);
	if(!cond) return TEST_FAIL;

	// Setting elements
	for(int i = 0; i != (int)a->size; ++i){
		r = array_set(a, &i, i);
		cond = cond && r;
	}
	if(!cond) return TEST_FAIL;

	// Getting elements
	cond = 1;
	for(int i = 0; i != (int)a->size; ++i){
		cond = cond && (*(int*)array_get(a, i) == i);
	}
	if(!cond) return TEST_FAIL;

	cond = (*(int*)array_front(a)==0) && (*(int*)array_back(a)==9);
	if(!cond) return TEST_FAIL;

	// Case: value passed is NULL
	r = array_set(a, NULL, 1);
	cond = (r) && (*(int*)array_get(a,1)==0);
	if(!cond) return TEST_FAIL;
	
	// Limiting case: index out of bounds
	r = array_set(a, NULL, a->size);
	cond = (!r);
	if(!cond) return TEST_FAIL;

	return TEST_PASS;
}


#define array_foreach(T, element, array) \
for(T element = array->data; element != array_end(array); element++)

int test_array_insert(){
	struct array_t *a, *r;
	int cond, value = 999, prev;

	a = array_create(sizeof(int));
	r = array_resize(a, 10);
	// Setting elements
	for(int i = 0; i != (int)a->size; ++i){
		r = array_set(a, &i, i);
	}

	array_foreach(int*, element, a){
		printf("%d\n", *element);
	}
	
	prev = *(int*)array_get(a, 5);
	r = array_insert(a, &value, 5);

	printf("v1=%d, v2=%d\n", *(int*)array_get(a,5), *(int*)array_get(a, 5));

	cond = r && (a->size == 11);
	cond = cond && (*(int*)array_get(a,5)==value) && (*(int*)array_get(a,6)==prev);
	if(!cond) return TEST_FAIL;

	return TEST_PASS;
}


void containers_test(){
	struct test_t tests[] = {
		{.fn = test_array_create_destroy, .name = "test_array_create_destroy"},
		{.fn = test_array_resize, .name = "test_array_resize"},
		{.fn = test_array_set_get, .name = "test_array_set_get"},
		{.fn = test_array_insert, .name = "test_array_insert"},
	};
	uint32_t count = sizeof(tests) / sizeof(struct test_t);

	run_tests(tests, count, "containers");
}

