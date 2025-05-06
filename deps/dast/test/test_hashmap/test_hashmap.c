#include "test_hashmap.h"



/* CONSTANTS */

#define START_SIZE 12 // Will be 13 which is the next prime
#define ACTUAL_START_SIZE 13

#define TEST_ALLOCATOR (dast_allocator_t){.alloc=test_malloc_wrapper, .realloc=test_realloc_wrapper, .free=test_free_wrapper}

/* STATIC FUNCTIONS */

static void* test_malloc_wrapper (dast_sz size)             { return test_malloc((size_t)size); }
static void* test_realloc_wrapper(void* block, dast_sz size){ return test_realloc(block, (size_t)size); }
static void  test_free_wrapper   (void* block)              {        test_free(block); }

static dast_u64 test_hash_fn(const void* data, dast_sz len){ (void) data, (void) len; return 0; }

static dast_bool test_key_u64_eq(const void* a, const void* b, dast_sz sz){
    assert_int_equal(sz, sizeof(dast_u64));
    return *(dast_u64*)a == *(dast_u64*)b;
}

/* PUBLIC FUNCTIONS */

void test_hashmap_init_free(void** state){
    (void)state;

#ifndef DAST_NO_STDLIB

    hashmap_t map;
    void* result = hashmap_init(&map, START_SIZE);

    assert_non_null(result);
    assert_int_equal(map.size, ACTUAL_START_SIZE);
    assert_non_null(map.table);
    assert_null(map.entries);

    hashmap_uninit(&map);

    assert_null(map.table);
    assert_int_equal(map.size, 0);

#endif
}

void test_hashmap_init_null(void** state){
    (void)state;
    void* result = hashmap_init(NULL, START_SIZE);
    assert_null(result);
}

void test_hashmap_init_zero_size(void** state){
    (void)state;
    hashmap_t map;
    dast_sz size = 0, next_prime = 2;
    void* result = hashmap_init_custom(&map, size, TEST_ALLOCATOR, dast_null, dast_null);

    assert_non_null(result);
    assert_int_equal(map.size, next_prime);
    assert_non_null(map.table);
    assert_null(map.entries);

    hashmap_uninit(&map);

    assert_null(map.table);
    assert_int_equal(map.size, 0);
}

void test_hashmap_init_custom_default(void** state){
    (void)state;

#ifndef DAST_NO_STDLIB
    hashmap_t map;
    hashmap_init_custom(&map, START_SIZE, (dast_allocator_t){0}, NULL, NULL);
    
    assert_memory_equal(&map.alloc, &DAST_DEFAULT_ALLOCATOR, sizeof(dast_allocator_t));
    assert_ptr_equal(map.hash_fn, hashmap_FNV1a64_hash);
    assert_ptr_equal(map.eq_fn, dast_memeq);
    
    hashmap_uninit(&map);
#endif
}

void test_hashmap_init_custom_alloc(void** state){
    (void)state;
    hashmap_t map;
    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, NULL, NULL);
    
    assert_memory_equal(&map.alloc, &TEST_ALLOCATOR, sizeof(dast_allocator_t));
    assert_ptr_equal(map.hash_fn, hashmap_FNV1a64_hash);
    assert_ptr_equal(map.eq_fn, dast_memeq);
    
    hashmap_uninit(&map);
}

void test_hashmap_init_custom_hash(void** state){
    (void)state;
    hashmap_t map;
    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, test_hash_fn, NULL);
    
    assert_memory_equal(&map.alloc, &TEST_ALLOCATOR, sizeof(dast_allocator_t));
    assert_ptr_equal(map.hash_fn, test_hash_fn);
    assert_ptr_equal(map.eq_fn, dast_memeq);
    
    hashmap_uninit(&map);
}

void test_hashmap_init_custom_cmp(void** state){
    (void)state;
    hashmap_t map;
    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, test_hash_fn, test_key_u64_eq);
    
    assert_memory_equal(&map.alloc, &TEST_ALLOCATOR, sizeof(dast_allocator_t));
    assert_ptr_equal(map.hash_fn, test_hash_fn);
    assert_ptr_equal(map.eq_fn, test_key_u64_eq);
    
    hashmap_uninit(&map);
}

void test_hashmap_setb(void** state){
    (void)state;
    const char key[] = "key";
    dast_sz key_len = sizeof(key);
    int data = 99;
    hashmap_t map;

    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);
    void* result = hashmap_setb(&map, key, key_len, &data);

    assert_ptr_equal(result, &map);
    assert_ptr_equal(hashmap_getb(&map, key, key_len), &data);

    hashmap_uninit(&map);
}


void test_hashmap_setb_null_value(void** state){
    (void)state;
    const char key[] = "key";
    dast_sz key_len = sizeof(key);
    hashmap_t map;

    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);
    void* result = hashmap_setb(&map, key, key_len, NULL);

    assert_ptr_equal(result, &map);
    assert_null(hashmap_getb(&map, key, key_len));
    assert_true(hashmap_has_keyb(&map, key, key_len));

    hashmap_uninit(&map);
}

void test_hashmap_setb_bad_key(void** state){
    (void)state;
    hashmap_t map;

    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);
    void* result = hashmap_setb(&map, NULL, 0, NULL);

    assert_null(result);

    hashmap_uninit(&map);
}

void test_hashmap_getb_none(void** state){
    (void)state;
    const char key[] = "key";
    dast_sz key_len = sizeof(key);
    hashmap_t map;

    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);

    void* result = hashmap_getb(&map, key, key_len);
    assert_null(result);

    hashmap_uninit(&map);
}

void test_hashmap_has_keyb(void** state){
    (void)state;
    const char key[] = "key";
    dast_sz key_len = sizeof(key);
    int data = 99;
    hashmap_t map;

    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);
    hashmap_setb(&map, key, key_len, &data);

    int has_key = hashmap_has_keyb(&map, key, key_len);
    assert_true(has_key);

    hashmap_uninit(&map);
}

void test_hashmap_has_keyb_false(void** state){
    (void)state;
    const char key[] = "key";
    dast_sz key_len = sizeof(key);
    hashmap_t map;

    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);

    int has_key = hashmap_has_keyb(&map, key, key_len);
    assert_false(has_key);

    hashmap_uninit(&map);
}

void test_hashmap_iterb(void** state){
    (void)state;

    dast_sz nkeys = 3;
    const char* keys[] = {"key1", "key2", "key3"};
    dast_sz key_len = 5;
    hashmap_t map;

    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);
    hashmap_setb(&map, keys[2], key_len, NULL);
    hashmap_setb(&map, keys[1], key_len, NULL);
    hashmap_setb(&map, keys[0], key_len, NULL);

    char* k = NULL;
    dast_sz s, counter = 0;

    while( (k = hashmap_iterb(&map, k, &s)) ){
        assert_int_equal(s, key_len);
        int matched = 0;
        for(dast_sz i = 0; i != nkeys; ++i){
            if (memcmp(k, keys[i], key_len)){
                matched = 1;
                break;
            }
        }
        assert_int_equal(matched, 1);
        counter++;
    }
    assert_int_equal(counter, nkeys);
    
    hashmap_uninit(&map);
}

void test_hashmap_iterb_empty(void** state){
    (void)state;

    hashmap_t map;
    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);

    char* k = NULL;
    dast_sz s, counter = 0;

    while( (k = hashmap_iterb(&map, k, &s)) ){
        counter++;
    }
    assert_int_equal(counter, 0);
    
    hashmap_uninit(&map);
}

void test_hashmap_set_str(void** state){
    (void)state;
    
    hashmap_t map;
    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);

    string_t key = string_scoped_lit("key");
    int value = 99;
    void* result = hashmap_set(&map, key, &value);

    assert_non_null(result);
    assert_ptr_equal(hashmap_get(&map, key), &value);

    hashmap_uninit(&map);    
}

void test_hashmap_get_str(void** state){
    (void)state;
    
    hashmap_t map;
    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);

    string_t key = string_scoped_lit("key");
    int value = 99;
    void* result = hashmap_set(&map, key, &value);

    assert_non_null(result);
    assert_ptr_equal(hashmap_get(&map, key), &value);

    hashmap_uninit(&map);    
}

void test_hashmap_has_key_str(void** state){
    (void)state;

    hashmap_t map;
    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);

    string_t key = string_scoped_lit("key");
    int value = 99;
    hashmap_set(&map, key, &value);

    assert_true(hashmap_has_key(&map, key));

    hashmap_uninit(&map); 
}

void test_hashmap_iter_str(void** state){
    (void)state;

    hashmap_t map;
    hashmap_init_custom(&map, START_SIZE, TEST_ALLOCATOR, dast_null, dast_null);
    dast_sz nkeys = 3;
    dast_sz key_len = 4; // not counting null-terminating char
    string_t keys[] = {string_scoped_lit("key1"), string_scoped_lit("key2"), string_scoped_lit("key3")};

    hashmap_set(&map, keys[0], NULL);
    hashmap_set(&map, keys[1], NULL);
    hashmap_set(&map, keys[2], NULL);
    
    string_t k = (string_t){0};
    dast_sz counter = 0;
    while( hashmap_iter(&map, &k) ){
        assert_int_equal(k.len, key_len);
        counter++;
    }
    assert_int_equal(counter, nkeys);

    hashmap_uninit(&map); 
}

