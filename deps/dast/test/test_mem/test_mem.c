#include "mem.h"
#include "test_mem.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>


static void* custom_alloc(dast_sz size){
    return test_malloc(size);
}

static void* custom_realloc(void* block, dast_sz newsize){
    return test_realloc(block, newsize);
}

static void custom_free(void* block){
    test_free(block);
}


void test_default_alloc(void** state){
    (void)state;
    dast_allocator_t alloc = dast_get_alloc();
#ifdef DAST_NO_STDLIB
    assert_ptr_equal(alloc.alloc,   dast_null);
    assert_ptr_equal(alloc.realloc, dast_null);
    assert_ptr_equal(alloc.free,    dast_null);
#else
    assert_ptr_equal(alloc.alloc,   malloc);
    assert_ptr_equal(alloc.realloc, realloc);
    assert_ptr_equal(alloc.free,    free);
#endif
}


void test_get_set_alloc(void** state){
    (void)state;
    dast_allocator_t custom = {custom_alloc, custom_realloc, custom_free};
    dast_set_alloc(custom);
    dast_allocator_t alloc = dast_get_alloc();
    assert_memory_equal(&alloc, &custom, sizeof(dast_allocator_t));
}

void test_fallback_alloc(void** state){
    (void)state;

    dast_allocator_t custom = {custom_alloc, custom_realloc, dast_null};
    dast_set_alloc(custom);
    dast_allocator_t alloc = dast_get_alloc();

#ifdef DAST_NO_STDLIB
    assert_ptr_equal(alloc.alloc,   dast_null);
    assert_ptr_equal(alloc.realloc, dast_null);
    assert_ptr_equal(alloc.free,    dast_null);
#else
    assert_ptr_equal(alloc.alloc,   malloc);
    assert_ptr_equal(alloc.realloc, realloc);
    assert_ptr_equal(alloc.free,    free);
#endif
}


void test_memeq(void** state){
    (void)state;
    const char a[] = "Test String";
    const char b[] = "Test String";
    const char c[] = "Fake String";

    assert_true(dast_memeq(a, b, sizeof a));
    assert_false(dast_memeq(a, c, sizeof a));

}

void test_memset(void** state){
    (void)state;
    const char result[] = "eeeeeeeeee";
    char buf[10];
    void* r;
    int val = (int)'e';

    r = dast_memset(buf, val, sizeof buf);

    assert_ptr_equal(r, buf);
    assert_memory_equal(buf, result, sizeof buf);
}

void test_memcpy(void**state){
    (void)state;
    const char a[] = "Test String";
    char b[sizeof a];

    char* r = dast_memcpy(b, a, sizeof a);
    assert_ptr_equal(r, b);
    assert_memory_equal(a, b, sizeof a);
}

void test_memmove(void**state){
    (void)state;
    const char a[] = "0123456789abcdefghij";
    const char result[] = "abcdefghijabcdefghij";
    char buf[20];
    size_t sz = 20;
    size_t copy_from = 10; 
    size_t copy_size = 10;
    dast_memcpy(buf, a, sz);

    char* r = dast_memmove(buf, buf + copy_from, copy_size);
    assert_ptr_equal(r, buf);
    assert_memory_equal(buf, result, sz);
}