#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/containers/str.h"
#include "test_string.h"

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

void test_string_from_len(void** state){
    (void)state;
    size_t len = 10;
    string_t s = string_from_len(len);

    assert_int_equal(s.len, len);
    assert_non_null(s.str);
    assert_int_equal(*(s.str + s.len), (char)0);

    string_free(&s);
}

void test_string_from_len_zero(void** state){
    (void)state;
    size_t len = 0;
    string_t s = string_from_len(len);

    assert_int_equal(s.len, len);
    assert_non_null(s.str);
    assert_int_equal(*(s.str), (char)0);

    string_free(&s);
}

void test_string_from_chars(void** state){
    (void)state;
    
    char chars[] = {'t', 'e', 's', 't'};
    size_t len = 4;

    string_t s = string_from_chars(chars, len);

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

    string_t s = string_from_chars(chars, len);

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
    string_t s = string_from_literal("string literal");

    assert_int_equal(s.len, sizeof("string literal")-1);
    assert_memory_equal(s.str, "string literal", s.len);

    string_free(&s);
}

void test_string_from_literal_empty(void** state){
    (void)state;
    string_t s = string_from_literal("");

    assert_int_equal(s.len, 0);
    assert_non_null(s.str);

    string_free(&s);
}
