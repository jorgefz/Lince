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

int test_array_performance(){

	array_t nums = array_create(sizeof(int));

	TEST_CLOCK_START(time);
    int end = 10000000;
	for(int i = 0; i != end; ++i){
        array_push_back(&nums, &i);
	}
	long int n_op = end;
	TEST_CLOCK_END(time, n_op);
	
    array_destroy(&nums);
	return TEST_PASS;
}

int test_array(){
	array_t *r = NULL;

	// Create with element size 0
	// Should fail all attempts to insert or manipulate via provided functions
	array_t z = array_create(0);
	r = array_resize(&z, 10);
	TEST_ASSERT(!r, "Array of element size zero was able to be resized");

	r = array_insert(&z, (void*)5, 0);
	TEST_ASSERT(!r, "Array of element size zero could insert an element");

	// Create with ints
	array_t a = array_create(sizeof(int));
	TEST_ASSERT((a.size==0) && (a.capacity==0) && (!a.data),
		"Failed to create array with elements of size int");

	// Resize
	r = array_resize(&a, 10);
	TEST_ASSERT(r, "Failed to resize array of ints from size 0 to 10");
	TEST_ASSERT((a.size==10) && (a.capacity==16) && (a.data),
		"Failed to resize array of ints from size 0 to 10");
	
	// Setting elements
	int res = 1;
	for(int i = 0; i != (int)a.size; ++i){
		res = res && array_set(&a, &i, i);
	}
	TEST_ASSERT(res, "Failed to set array elements incrementally from 0 to 10");

	// Getting elements
	res = 1;
	for(int i = 0; i != (int)a.size; ++i){
		res = res && (*(int*)array_get(&a, i) == i);
	}
	TEST_ASSERT(res, "Failed to retrieve items, or retrieved item is wrong")

	res = (*(int*)array_front(&a)==0) && (*(int*)array_back(&a)==9);
	TEST_ASSERT(res, "Wrong front or back value");

	// Case: value passed is NULL
	r = array_set(&a, NULL, 1);
	res = (r) && (*(int*)array_get(&a,1) == 0);
	TEST_ASSERT(res, "Set value to NULL does not zero the value");
	
	// Limiting case: index out of bounds
	r = array_set(&a, NULL, a.size);
	TEST_ASSERT(!r, "Setting out-of-bounds value does not return NULL");

	// Inserting a value
	int prev = *(int*)array_get(&a, 5);
	int value = 99;
	r = array_insert(&a, &value, 5);

	res = r && (a.size == 11);
	res = res && (*(int*)array_get(&a,5)==value) && (*(int*)array_get(&a,6)==prev);
	TEST_ASSERT(res, "Failed to insert value");
	
	// Removing a value
	r = array_remove(&a, 1);
	r = array_pop_back(&a);
	r = array_pop_front(&a);

	res = r && (a.size == 8)
			&& (*(int*)array_front(&a)==2)
			&& (*(int*)array_back(&a)==8);
	TEST_ASSERT(res, "Failed to pop first or last value");

	// array_clear
	r = array_clear(&a);
	TEST_ASSERT(r && a.size==0, "Failed to clear array");

	array_destroy(&a);
	TEST_ASSERT(a.size==0 && a.capacity==0 && a.data==NULL && a.element_size==0,
		"Failed to destroy array");
	
	return TEST_PASS;
}
