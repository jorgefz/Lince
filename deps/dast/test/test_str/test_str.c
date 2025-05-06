#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "str.h"
#include "test_str.h"

static void* test_malloc_wrapper (dast_sz size)             { return test_malloc((size_t)size); }
static void* test_realloc_wrapper(void* block, dast_sz size){ return test_realloc(block, (size_t)size); }
static void  test_free_wrapper   (void* block)              {        test_free(block); }

#define TEST_ALLOCATOR (dast_allocator_t){.alloc=test_malloc_wrapper, .realloc=test_realloc_wrapper, .free=test_free_wrapper}


void test_string_scoped(void** state){
    (void)state;
    const char* chars = "test string";
    size_t len = sizeof(chars) - 1;
    string_t s = string_scoped(chars, len);

    assert_ptr_equal(s.str, chars);
    assert_int_equal(s.len, len);
}

void test_string_scoped_lit(void** state){
    (void)state;
    const char literal[] = "test string";
    size_t len = sizeof(literal) - 1;
    string_t s = string_scoped_lit(literal);

    assert_ptr_equal(s.str, literal);
    assert_int_equal(s.len, len);
}

void test_string_default_alloc(void** state){
    (void)state;

#ifndef DAST_NO_STDLIB
    size_t len = 10;
    string_t s = string_from_len(len);

    assert_int_equal(s.len, len);
    assert_non_null(s.str);
    assert_int_equal(*(s.str + s.len), (char)0);

    string_free(&s);
#endif
}

void test_string_get_alloc(void** state){
    (void)state;

    string_t s = string_from_len_custom(8, TEST_ALLOCATOR);
    dast_allocator_t* alloc = string_get_alloc(s);

    assert_memory_equal(alloc, &TEST_ALLOCATOR, sizeof(dast_allocator_t));
    assert_ptr_equal(alloc + 1, s.str);

    string_free(&s);
}

void test_string_from_len(void** state){
    (void)state;
    size_t len = 10;
    string_t s = string_from_len_custom(len, TEST_ALLOCATOR);

    assert_int_equal(s.len, len);
    assert_non_null(s.str);
    assert_int_equal(*(s.str + s.len), (char)0);

    string_free(&s);
}

void test_string_from_len_zero(void** state){
    (void)state;
    size_t len = 0;
    string_t s = string_from_len_custom(len, TEST_ALLOCATOR);

    assert_int_equal(s.len, len);
    assert_non_null(s.str);
    assert_int_equal(*(s.str), (char)0);

    string_free(&s);
}

void test_string_from_chars(void** state){
    (void)state;
    
    char chars[] = {'t', 'e', 's', 't'};
    size_t len = 4;

    string_t s = string_from_chars_custom(chars, len, TEST_ALLOCATOR);

    assert_int_equal(s.len, len);
    assert_non_null(s.str);
    assert_memory_equal(s.str, chars, len);
    assert_int_equal(*(s.str + s.len), (char)0);

    string_free(&s);
}

void test_string_from_chars_null(void** state){
    (void)state;
    
    const char* chars = NULL;
    size_t len = 4;

    string_t s = string_from_chars_custom(chars, len, TEST_ALLOCATOR);

    assert_int_equal(s.len, len);
    assert_non_null(s.str);
    assert_int_equal(*(s.str + s.len), (char)0);

    string_free(&s);
}

void test_string_free(void** state){
    (void)state;
    size_t len = 10;
    string_t s = string_from_len(len);

    assert_int_equal(s.len, len);
    assert_non_null(s.str);

    string_free(&s);

    assert_int_equal(s.len, 0);
    assert_null(s.str);
}

void test_string_from_literal(void** state){
    (void)state;
    string_t s = string_from_literal_custom("string literal", TEST_ALLOCATOR);

    assert_int_equal(s.len, sizeof("string literal")-1);
    assert_memory_equal(s.str, "string literal", s.len);

    string_free(&s);
}

void test_string_from_literal_empty(void** state){
    (void)state;
    string_t s = string_from_literal_custom("", TEST_ALLOCATOR);

    assert_int_equal(s.len, 0);
    assert_non_null(s.str);

    string_free(&s);
}

void test_string_from_fmt(void** state){
    (void)state;
    
#ifndef DAST_NO_STDLIB
    int fmt_int = 42;
    const char* fmt_str = "hello";
    const char result[] = "42 hello";
    size_t result_len = sizeof(result) - 1;

    string_t s = string_from_fmt_custom(TEST_ALLOCATOR, "%d %s", fmt_int, fmt_str);

    assert_non_null(s.str);
    assert_int_equal(s.len, result_len);
    assert_memory_equal(s.str, result, result_len);

    string_free(&s);
#endif
}


void test_string_copy_custom(void** state){
    (void)state;

    string_t s1 = string_from_literal_custom("TEST", TEST_ALLOCATOR);
    string_t s2 = string_copy_custom(s1, TEST_ALLOCATOR);

    assert_int_equal(s1.len, s2.len);
    assert_memory_equal(s1.str, s2.str, s1.len);

    string_free(&s1);
    string_free(&s2);
}


void test_string_copy(void** state){
    (void)state;

    string_t s1 = string_from_literal_custom("TEST", TEST_ALLOCATOR);
    string_t s2 = string_copy(s1);

    assert_int_equal(s1.len, s2.len);
    assert_memory_equal(s1.str, s2.str, s1.len);
    assert_memory_equal(string_get_alloc(s1), string_get_alloc(s2), sizeof(dast_allocator_t));

    string_free(&s1);
    string_free(&s2);
}

