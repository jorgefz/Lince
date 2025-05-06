
#include "mem.h"
#include "array.h"
#include "test_array.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

#define TEST_ALLOCATOR (dast_allocator_t){malloc_wrap, realloc_wrap, free}

void* malloc_wrap(dast_sz bytes){
	return malloc((size_t)bytes);
}

void* realloc_wrap(void* block, dast_sz bytes){
	return realloc(block, (size_t)bytes);
}



// Verifies array is initialised properly
void test_array_init(void** state){
	(void)state;
	array_t a, *success;
#ifdef DAST_NO_STDLIB
	success = array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
#else
	success = array_init(&a, sizeof(int));
#endif
	assert_non_null(success);
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
	array_t a, *success;
	success = array_init_custom(&a, 0, TEST_ALLOCATOR);
	assert_null(success);
}

// Verifies an array fails to initialise
// when the given pointer is null
void test_array_init_null(void** state){
	(void)state;
	array_t* success;
	success = array_init_custom(NULL, sizeof(int), TEST_ALLOCATOR);
	assert_null(success);
}

// Verifies an array is resized properly
// to a larger number of items
void test_array_resize_larger(void** state){
	(void)state;
	array_t a;
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
	uint32_t size = 1;
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
	array_resize(&a, size);
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
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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

	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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

// Verifies all elements of the array are removed
void test_array_clear(void** state){
	(void)state;
	array_t a;
	uint32_t size = 2;

	array_init_custom(&a, sizeof(int), TEST_ALLOCATOR);
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
void test_array_iter_index(void** state){
	(void)state;
	array_t a;
	uint32_t size = 10;

	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);
	array_resize(&a, size);

	for(uint32_t i = 0; i != size; ++i){
		uint32_t* item = array_set(&a, &i, i);
		assert_ptr_equal(item, (char*)a.data + i*a.element_size);
		assert_int_equal(*item, i);
	}

	array_uninit(&a);
}

// Verifies an array is iterated via begin/end pointers
void test_array_iter_ptr(void** state){
	(void)state;
	array_t a;
	uint32_t size = 10;

	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);
	array_resize(&a, size);
	for(uint32_t i = 0; i != size; ++i) array_set(&a, &i, i);

	uint32_t c = 0;
	for(uint32_t *item = a.begin; item != a.end; ++item){
		assert_ptr_equal(item, (char*)a.data + c * a.element_size);
		assert_int_equal(*item, c);
		assert_true((char*)item <= (char*)a.data + a.size * a.element_size);
		c++;
	}

	array_uninit(&a);
}

// Verifies an array is iterated via begin/end pointers
// on an array with zero elements
void test_array_iter_ptr_zero(void** state){
	(void)state;
	array_t a;
	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);

	for(uint32_t *item = a.begin; item != a.end; ++item){
		assert_true(0); // This code should never run
	}

	array_uninit(&a);
}

// Verifies an element is inserted
void test_array_insert(void** state){
	(void)state;
	array_t a;
	uint32_t size = 10;
	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);
	array_resize(&a, size);

	uint32_t v = 123;
	uint32_t insert_index = 5;
	uint32_t* result = array_insert(&a, &v, insert_index);

	assert_non_null(result);
	assert_ptr_equal(result, array_get(&a, insert_index));
	assert_int_equal(*result, v);
	assert_int_equal(a.size, size+1);

	array_uninit(&a);
}

// Verifies an element is not inserted
// when the given index is out of bounds
void test_array_insert_out_of_bounds(void** state){
	(void)state;
	array_t a;
	uint32_t size = 10;
	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);
	array_resize(&a, size);

	uint32_t v = 123;
	uint32_t insert_index = size + 1;
	uint32_t* result = array_insert(&a, &v, insert_index);

	assert_null(result);
	assert_int_equal(size, a.size);

	array_uninit(&a);
}

// Verifies an element is inserted with a value of zero
// when the given pointer-to-value is null
void test_array_insert_null(void** state){
	(void)state;
	array_t a;
	uint32_t size = 10;
	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);
	array_resize(&a, size);

	uint32_t insert_index = 5;
	uint32_t* result = array_insert(&a, NULL, insert_index);

	assert_non_null(result);
	assert_ptr_equal(result, array_get(&a, insert_index));
	assert_int_equal(*result, 0);
	assert_int_equal(a.size, size+1);

	array_uninit(&a);
}

// Verifies an element is inserted on an empty array
void test_array_insert_empty(void** state){
	(void)state;
	array_t a;
	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);

	uint32_t v = 123;
	uint32_t insert_index = 0;
	uint32_t* result = array_insert(&a, &v, insert_index);

	assert_non_null(result);
	assert_ptr_equal(result, a.data);
	assert_int_equal(a.size, 1);
	assert_int_equal(*result, v);

	array_uninit(&a);
}

// Verifies an element is inserted
// after the last item
void test_array_push_back(void** state){
	(void)state;
	array_t a;
	uint32_t size = 5;
	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);
	array_resize(&a, size);

	uint32_t v = 123;
	uint32_t* result = array_push_back(&a, &v);

	assert_non_null(result);
	assert_ptr_equal(result, array_back(&a));
	assert_int_equal(a.size, size + 1);
	assert_int_equal(*result, v);

	array_uninit(&a);
}

// Verifies an element is inserted
// before the first item
void test_array_push_front(void** state){
	(void)state;
	array_t a;
	uint32_t size = 5;
	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);
	array_resize(&a, size);

	uint32_t v = 123;
	uint32_t* result = array_push_front(&a, &v);

	assert_non_null(result);
	assert_ptr_equal(result, array_front(&a));
	assert_int_equal(a.size, size + 1);
	assert_int_equal(*result, v);

	array_uninit(&a);
}

// Verifies an element is removed from the array
void test_array_remove(void** state){
	(void)state;
	array_t a;
	uint32_t size = 5;
	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);
	array_resize(&a, size);
	for(uint32_t i=0, *item = a.begin; item != a.end; ++item) *item = i++;

	uint32_t remove_index = 2;
	uint32_t v_at_index = *(uint32_t*)array_get(&a, remove_index);
	array_t* result = array_remove(&a, remove_index);

	assert_non_null(result);
	assert_ptr_equal(result, &a);
	assert_int_equal(a.size, size - 1);
	assert_int_equal(v_at_index, *(uint32_t*)array_get(&a, remove_index) - 1);

	array_uninit(&a);
}

// Verifies an element is removed from the array
void test_array_remove_zero(void** state){
	(void)state;
	array_t a;
	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);
	
	uint32_t remove_index = 1;
	array_t* result = array_remove(&a, remove_index);

	assert_null(result);
	assert_int_equal(a.size, 0);

	array_uninit(&a);
}

// Verifies the last item is removed
void test_array_pop_back(void** state){
	(void)state;
	array_t a;
	uint32_t size = 5;
	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);
	array_resize(&a, size);
	for(uint32_t i=0, *item = a.begin; item != a.end; ++item) *item = i++;

	uint32_t v_at_back = *(uint32_t*)array_back(&a);
	array_t* result = array_pop_back(&a);
	
	assert_non_null(result);
	assert_ptr_equal(result, &a);
	assert_int_equal(a.size, size - 1);
	assert_int_equal(v_at_back, *(uint32_t*)(array_back(&a)) + 1);

	array_uninit(&a);
}

// Verifies the first item is removed
void test_array_pop_front(void** state){
	(void)state;
	array_t a;
	uint32_t size = 5;
	array_init_custom(&a, sizeof(uint32_t), TEST_ALLOCATOR);
	array_resize(&a, size);
	for(uint32_t i=0, *item = a.begin; item != a.end; ++item) *item = i++;

	uint32_t v_at_front = *(uint32_t*)array_front(&a);
	array_t* result = array_pop_front(&a);

	assert_non_null(result);
	assert_ptr_equal(result, &a);
	assert_int_equal(a.size, size - 1);
	assert_int_equal(v_at_front, *(uint32_t*)array_front(&a) - 1);

	array_uninit(&a);
}

