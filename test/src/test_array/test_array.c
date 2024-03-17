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



// Verifies array is initialised properly
void test_array_init(void** state){
	(void)state;
	array_t a;
	int success = array_init(&a, sizeof(int));
	assert_int_equal(success, 1);
	assert_int_equal(a.size, 0);
	assert_int_equal(a.element_size, sizeof(int));
	assert_int_equal(a.capacity, 0);
	assert_null(a.data);
	assert_null(a.begin);
	assert_null(a.end);
	array_uninit(&a);
}

// Verifies an array fails to initialise
// when the given element size is zero
void test_array_init_zero_element_size(void** state){
	(void)state;
	array_t a;
	int success = array_init(&a, 0);
	assert_int_equal(success, 0);
	assert_int_equal(a.size, 0);
	assert_int_equal(a.element_size, 0);
	assert_int_equal(a.capacity, 0);
	assert_null(a.data);
	assert_null(a.begin);
	assert_null(a.end);
	array_uninit(&a);
}

// Verifies an array fails to initialise
// when the given pointer is null
void test_array_init_null(void** state){
	(void)state;
	int success = array_init(NULL, sizeof(int));
	assert_int_equal(success, 0);
}

// Verifies an array is allocated and initialised properly
void test_array_create(void** state){
	(void)state;
	array_t* a = array_create(sizeof(int));
	assert_non_null(a);
	assert_int_equal(a->size, 0);
	assert_int_equal(a->element_size, sizeof(int));
	assert_int_equal(a->capacity, 0);
	assert_null(a->data);
	assert_null(a->begin);
	assert_null(a->end);
	array_destroy(a);
}

// Verifies an array fails to be created
// when the given element size is zero
void test_array_create_zero_element_size(void** state){
	(void)state;
	array_t* a = array_create(0);
	assert_null(a);
}

// Verifies an array is resized properly
// to a larger number of items
void test_array_resize_larger(void** state){
	(void)state;
	array_t a;
	array_init(&a, sizeof(int));
	uint32_t new_size = 5;
	uint32_t new_capacity = 8; // Nearest power of two
	void* success = array_resize(&a, new_size);
	assert_non_null(success);
	assert_int_equal(a.size, new_size);
	assert_int_equal(a.element_size, sizeof(int));
	assert_int_equal(a.capacity, new_capacity);
	assert_non_null(a.data);
	assert_ptr_equal(a.begin, a.data);
	assert_ptr_equal(a.end, (char*)a.data + new_size * sizeof(int));
	array_uninit(&a);
}

// Verifies an array is resized properly
// to a smaller number of items
void test_array_resize_smaller(void** state){
	(void)state;
	array_t a;
	array_init(&a, sizeof(int));
	uint32_t new_size = 5;
	uint32_t new_capacity = 8; // Nearest power of two
	array_resize(&a, new_size);

	uint32_t smaller_size = 3;
	void* success = array_resize(&a, smaller_size);

	assert_non_null(success);
	assert_int_equal(a.size, smaller_size);
	assert_int_equal(a.capacity, new_capacity); // capacity stays the same
	assert_int_equal(a.element_size, sizeof(int));
	assert_non_null(a.data);
	assert_ptr_equal(a.begin, a.data);
	assert_ptr_equal(a.end, (char*)a.data + smaller_size * sizeof(int));
	array_uninit(&a);
}

// Verifies an array is not resized
// when the same number of items is given
void test_array_resize_same(void** state){
	(void)state;
	array_t a;
	array_init(&a, sizeof(int));
	uint32_t new_size = 5;
	uint32_t new_capacity = 8; // Nearest power of two
	array_resize(&a, new_size);

	void* success = array_resize(&a, new_size);

	assert_non_null(success);
	assert_int_equal(a.size, new_size);
	assert_int_equal(a.capacity, new_capacity);
	assert_int_equal(a.element_size, sizeof(int));
	assert_non_null(a.data);
	assert_ptr_equal(a.begin, a.data);
	assert_ptr_equal(a.end, (char*)a.data + new_size * sizeof(int));
	array_uninit(&a);
}

// Verifies an array is resized properly
// to a size of zero items
void test_array_resize_zero(void** state){
	(void)state;
	array_t a;
	array_init(&a, sizeof(int));
	uint32_t new_size = 5;
	uint32_t new_capacity = 8; // Nearest power of two
	array_resize(&a, new_size);

	void* success = array_resize(&a, 0);

	assert_non_null(success);
	assert_int_equal(a.size, 0);
	assert_int_equal(a.capacity, new_capacity);
	assert_int_equal(a.element_size, sizeof(int));
	assert_non_null(a.data);
	assert_null(a.begin);
	assert_null(a.end);
	array_uninit(&a);
}

// Verifies an array is unitialised properly
void test_array_uninit(void** state){
	(void)state;
	array_t a;
	array_init(&a, sizeof(int));
	array_resize(&a, 10);
	array_uninit(&a);

	assert_int_equal(a.size, 0);
	assert_int_equal(a.capacity, 0);
	assert_int_equal(a.element_size, 0);
	assert_null(a.data);
}

// Verifies an array element is set properly
void test_array_set(void** state){
	(void)state;
	array_t a;
	int v = 8;
	array_init(&a, sizeof(int));
	array_resize(&a, 1);
	int* item = array_set(&a, &v, 0);

	assert_non_null(item);
	assert_int_equal(*item, v);
	assert_ptr_equal(item, a.data);

	array_uninit(&a);
}

// Verifies an array element is zeroed properly
// when the given pointer-to-value is null
void test_array_set_null(void** state){
	(void)state;
	array_t a;
	array_init(&a, sizeof(int));
	array_resize(&a, 1);
	int* item = array_set(&a, NULL, 0);

	assert_non_null(item);
	assert_int_equal(*item, 0);
	assert_ptr_equal(item, a.data);

	array_uninit(&a);
}

// Verifies an array element is not set
// when the given index is out of bounds
void test_array_set_out_of_bounds(void** state){
	(void)state;
	array_t a;
	uint32_t size = 5;
	int v = 8;
	array_init(&a, sizeof(int));
	array_resize(&a, size);
	int* item = array_set(&a, &v, size);
	assert_null(item);
	array_uninit(&a);
}

// Verifies an array element is retrieved properly
void test_array_get(void** state){
	(void)state;
	array_t a;
	int v = 8;
	array_init(&a, sizeof(int));
	array_resize(&a, 1);
	int* set = array_set(&a, &v, 0);
	int* get = array_get(&a, 0);

	assert_non_null(get);
	assert_ptr_equal(get, set);
	assert_int_equal(*get, v);
	assert_ptr_equal(get, a.data);

	array_uninit(&a);
}

// Verifies an array element is not retrieved
// when the given index is out of bounds
void test_array_get_out_of_bounds(void** state){
	(void)state;
	array_t a;
	int v = 8;
	uint32_t size = 1;
	array_init(&a, sizeof(int));
	array_resize(&a, size);
	int* set = array_set(&a, &v, size-1);
	int* get = array_get(&a, size);
	assert_null(get);
	array_uninit(&a);
}

// Verifies the front, back, and end of
// an array are properly calculated
void test_array_front_back_end(void** state){
	(void)state;
	array_t a;
	uint32_t size = 5;
	array_init(&a, sizeof(int));
	array_resize(&a, size);
	void* front = array_front(&a);
	void* back = array_back(&a);
	void* end = array_end(&a);

	assert_non_null(front);
	assert_ptr_equal(front, a.data);
	assert_non_null(back);
	assert_ptr_equal(back, (char*)a.data + (a.size - 1) * a.element_size);
	assert_non_null(end);
	assert_ptr_equal(end, (char*)a.data + a.size * a.element_size);

	array_uninit(&a);
}

// Verifies the front, back, and end of
// an array are properly calculated
// when the array is zero sized
void test_array_front_back_end_zero(void** state){
(void)state;
	array_t a;
	array_init(&a, sizeof(int));
	void* front = array_front(&a);
	void* back = array_back(&a);
	void* end = array_end(&a);

	assert_null(front);
	assert_null(back);
	assert_null(end);

	array_uninit(&a);
}

// Verifies an array is copied
void test_array_copy(void** state){
	(void)state;
	array_t a, b;
	uint32_t size = 2;
	int v1 = 100, v2 = -100;

	array_init(&a, sizeof(int));
	array_resize(&a, size);
	array_set(&a, &v1, 0);
	array_set(&a, &v2, 1);

	void* result = array_copy(&b, &a);

	assert_non_null(result);
	assert_ptr_equal(result, &b);
	assert_int_equal(a.size, b.size);
	assert_int_equal(a.element_size, b.element_size);
	assert_ptr_equal((char*)a.begin-(char*)a.data, (char*)b.begin-(char*)b.data);
	assert_ptr_equal((char*)a.end-(char*)a.data, (char*)b.end-(char*)b.data);
	assert_memory_equal(a.data, b.data, b.size * b.element_size);

	array_uninit(&a);
	array_uninit(&b);
}

// Verifies an array is copied
void test_array_copy_zero(void** state){
	(void)state;
	array_t a, b;

	array_init(&a, sizeof(int));

	void* result = array_copy(&b, &a);

	assert_non_null(result);
	assert_ptr_equal(result, &b);
	assert_int_equal(a.size, b.size);
	assert_int_equal(a.element_size, b.element_size);
	assert_null(b.begin);
	assert_null(b.end);

	array_uninit(&a);
	array_uninit(&b);
}

// Verifies a heap-allocated copy of an array is returned
void test_array_new_copy(void** state){
	(void)state;
	array_t a;
	uint32_t size = 2;
	int v1 = 100, v2 = -100;

	array_init(&a, sizeof(int));
	array_resize(&a, size);
	array_set(&a, &v1, 0);
	array_set(&a, &v2, 1);
	array_t* b = array_new_copy(&a);

	assert_non_null(b);
	assert_int_equal(a.size, b->size);
	assert_int_equal(a.element_size, b->element_size);
	assert_ptr_equal((char*)a.begin-(char*)a.data, (char*)b->begin-(char*)b->data);
	assert_ptr_equal((char*)a.end-(char*)a.data, (char*)b->end-(char*)b->data);
	assert_memory_equal(a.data, b->data, b->size * b->element_size);

	array_uninit(&a);
	array_destroy(b);
}

// Verifies a heap-allocated copy of an array is returned
void test_array_new_copy_zero(void** state){
	(void)state;
	array_t a;
	array_init(&a, sizeof(int));
	array_t* b = array_new_copy(&a);

	assert_non_null(b);
	assert_int_equal(a.size, b->size);
	assert_int_equal(a.element_size, b->element_size);
	assert_null(b->begin);
	assert_null(b->end);

	array_uninit(&a);
	array_destroy(b);
}

// Verifies all elements of the array are removed
void test_array_clear(void** state){
	(void)state;
	array_t a;
	uint32_t size = 2;

	array_init(&a, sizeof(int));
	array_resize(&a, size);
	void* r = array_clear(&a);

	assert_non_null(r);
	assert_ptr_equal(r, &a);
	assert_int_equal(a.size, 0);
	assert_null(a.begin);
	assert_null(a.end);

	array_uninit(&a);
}

// Verifies an array is iterated via index
void test_array_iter_index(void** state){}

// Verifies an array is iterated via begin/end pointers
void test_array_iter_ptr(void** state){}

// Verifies an array is iterated via begin/end pointers
// on an array with zero elements
void test_array_iter_ptr_zero(void** state){}

// Verifies an element is inserted
void test_array_insert(void** state){}

// Verifies an element is not inserted
// when the given index is out of bounds
void test_array_insert_out_of_bounds(void** state){}

// Verifies an element is inserted with a value of zero
// when the given pointer-to-value is null
void test_array_insert_null(void** state){}

// Verifies an element is inserted
// after the last item
void test_array_push_back(void** state){}

// Verifies an element is inserted
// before the first item
void test_array_push_front(void** state){}

// Verifies an element is removed from the array
void test_array_remove(void** state){}

// Verifies an element is removed from the array
void test_array_remove_zero(void** state){}

// Verifies the last item is removed
void test_array_pop_back(void** state){}

// Verifies the first item is removed
void test_array_pop_front(void** state){}

