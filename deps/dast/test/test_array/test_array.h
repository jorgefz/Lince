#ifndef TEST_ARRAY
#define TEST_ARRAY

#define TEST_GROUP_ARRAY \
    cmocka_unit_test(test_array_init), \
    cmocka_unit_test(test_array_init_zero_element_size), \
    cmocka_unit_test(test_array_init_null), \
    cmocka_unit_test(test_array_resize_larger), \
    cmocka_unit_test(test_array_resize_smaller), \
    cmocka_unit_test(test_array_resize_same), \
    cmocka_unit_test(test_array_resize_zero), \
    cmocka_unit_test(test_array_uninit), \
    cmocka_unit_test(test_array_set), \
    cmocka_unit_test(test_array_set_null), \
    cmocka_unit_test(test_array_set_out_of_bounds), \
    cmocka_unit_test(test_array_get), \
    cmocka_unit_test(test_array_get_out_of_bounds), \
    cmocka_unit_test(test_array_front_back_end), \
    cmocka_unit_test(test_array_front_back_end_zero), \
    cmocka_unit_test(test_array_copy), \
    cmocka_unit_test(test_array_copy_zero), \
    cmocka_unit_test(test_array_clear), \
    cmocka_unit_test(test_array_iter_index), \
    cmocka_unit_test(test_array_iter_ptr), \
    cmocka_unit_test(test_array_iter_ptr_zero), \
    cmocka_unit_test(test_array_insert), \
    cmocka_unit_test(test_array_insert_out_of_bounds), \
    cmocka_unit_test(test_array_insert_null), \
    cmocka_unit_test(test_array_insert_empty), \
    cmocka_unit_test(test_array_push_back), \
    cmocka_unit_test(test_array_push_front), \
    cmocka_unit_test(test_array_remove), \
    cmocka_unit_test(test_array_remove_zero), \
    cmocka_unit_test(test_array_pop_back), \
    cmocka_unit_test(test_array_pop_front)


// Verifies array is initialised properly
void test_array_init(void** state);

// Verifies an array fails to initialise
// when the given element size is zero
void test_array_init_zero_element_size(void** state);

// Verifies an array fails to initialise
// when the given pointer is null
void test_array_init_null(void** state);

// Verifies an array is resized properly
// to a larger number of items
void test_array_resize_larger(void** state);

// Verifies an array is resized properly
// to a smaller number of items
void test_array_resize_smaller(void** state);

// Verifies an array is not resized
// when the same number of items is given
void test_array_resize_same(void** state);

// Verifies an array is resized properly
// to a size of zero items
void test_array_resize_zero(void** state);

// Verifies an array is unitialised properly
void test_array_uninit(void** state);

// Verifies an array element is set properly
void test_array_set(void** state);

// Verifies an array element is zeroed properly
// when the given pointer-to-value is null
void test_array_set_null(void** state);

// Verifies an array element is not set
// when the given index is out of bounds
void test_array_set_out_of_bounds(void** state);

// Verifies an array element is retrieved properly
void test_array_get(void** state);

// Verifies an array element is not retrieved
// when the given index is out of bounds
void test_array_get_out_of_bounds(void** state);

// Verifies the front, back, and end of
// an array are properly calculated
void test_array_front_back_end(void** state);

// Verifies the front, back, and end of
// an array are properly calculated
// when the array is zero sized
void test_array_front_back_end_zero(void** state);

// Verifies a heap-allocated copy of an array is returned
void test_array_copy(void** state);

// Verifies an array is copied
void test_array_copy_zero(void** state);

// Verifies all elements of the array are removed
void test_array_clear(void** state);

// Verifies an array is iterated via index
void test_array_iter_index(void** state);

// Verifies an array is iterated via begin/end pointers
void test_array_iter_ptr(void** state);

// Verifies an array is iterated via begin/end pointers
// on an array with zero elements
void test_array_iter_ptr_zero(void** state);

// Verifies an element is inserted
void test_array_insert(void** state);

// Verifies an element is not inserted
// when the given index is out of bounds
void test_array_insert_out_of_bounds(void** state);

// Verifies an element is inserted with a value of zero
// when the given pointer-to-value is null
void test_array_insert_null(void** state);

// Verifies an element is inserted on an empty array
void test_array_insert_empty(void** state);

// Verifies an element is inserted
// after the last item
void test_array_push_back(void** state);

// Verifies an element is inserted
// before the first item
void test_array_push_front(void** state);

// Verifies an element is removed from the array
void test_array_remove(void** state);

// Verifies an element is removed from the array
void test_array_remove_zero(void** state);

// Verifies the last item is removed
void test_array_pop_back(void** state);

// Verifies the first item is removed
void test_array_pop_front(void** state);


#endif /* TEST_ARRAY */