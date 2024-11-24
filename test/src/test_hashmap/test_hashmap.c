#include "test_hashmap.h"

void test_hashmap_init_free(void** state){
    (void)state;
    hashmap_t map;
    uint64_t size = 10;
    uint64_t next_prime = 11;
    void* result = hashmap_init(&map, size);

    assert_non_null(result);
    assert_int_equal(map.size, next_prime);
    assert_non_null(map.table);
    assert_null(map.entries);

    hashmap_uninit(&map);

    assert_null(map.table);
    assert_int_equal(map.size, 0);
}

void test_hashmap_init_null(void** state){
    (void)state;
    uint64_t size = 10;
    void* result = hashmap_init(NULL, size);
    assert_null(result);
}

void test_hashmap_init_zero_size(void** state){
    (void)state;
    hashmap_t map;
    uint64_t size = 0;
    uint64_t next_prime = 2;
    void* result = hashmap_init(&map, size);

    assert_non_null(result);
    assert_int_equal(map.size, next_prime);
    assert_non_null(map.table);
    assert_null(map.entries);

    hashmap_uninit(&map);

    assert_null(map.table);
    assert_int_equal(map.size, 0);
}

void test_hashmap_hash(void** state){
    (void)state;
    const char key[] = "key";
    size_t key_len = sizeof(key);
    size_t map_size = 17;

    uint32_t hash = hashmap_hashb(key, key_len, map_size);
    assert_true(hash < map_size);
}

void test_hashmap_setb(void** state){
    (void)state;
    const char key[] = "key";
    size_t key_len = sizeof(key);
    int data = 99;
    hashmap_t map;

    hashmap_init(&map, 1);
    void* result = hashmap_setb(&map, key, key_len, &data);

    assert_ptr_equal(result, &map);
    assert_ptr_equal(hashmap_getb(&map, key, key_len), &data);

    hashmap_uninit(&map);
}


void test_hashmap_setb_null_value(void** state){
    (void)state;
    const char key[] = "key";
    size_t key_len = sizeof(key);
    hashmap_t map;

    hashmap_init(&map, 1);
    void* result = hashmap_setb(&map, key, key_len, NULL);

    assert_ptr_equal(result, &map);
    assert_null(hashmap_getb(&map, key, key_len));
    assert_true(hashmap_has_keyb(&map, key, key_len));

    hashmap_uninit(&map);
}

void test_hashmap_setb_bad_key(void** state){
    (void)state;
    hashmap_t map;

    hashmap_init(&map, 1);
    void* result = hashmap_setb(&map, NULL, 0, NULL);

    assert_null(result);

    hashmap_uninit(&map);
}

void test_hashmap_getb_none(void** state){
    (void)state;
    const char key[] = "key";
    size_t key_len = sizeof(key);
    hashmap_t map;

    hashmap_init(&map, 1);

    void* result = hashmap_getb(&map, key, key_len);
    assert_null(result);

    hashmap_uninit(&map);
}

void test_hashmap_has_keyb(void** state){
    (void)state;
    const char key[] = "key";
    size_t key_len = sizeof(key);
    int data = 99;
    hashmap_t map;

    hashmap_init(&map, 1);
    hashmap_setb(&map, key, key_len, &data);

    int has_key = hashmap_has_keyb(&map, key, key_len);
    assert_true(has_key);

    hashmap_uninit(&map);
}

void test_hashmap_has_keyb_false(void** state){
    (void)state;
    const char key[] = "key";
    size_t key_len = sizeof(key);
    hashmap_t map;

    hashmap_init(&map, 1);

    int has_key = hashmap_has_keyb(&map, key, key_len);
    assert_false(has_key);

    hashmap_uninit(&map);
}

void test_hashmap_iterb(void** state){
    (void)state;

    size_t nkeys = 3;
    const char* keys[] = {"key1", "key2", "key3"};
    size_t key_len = 5;
    hashmap_t map;

    hashmap_init(&map, 1);
    hashmap_setb(&map, keys[2], key_len, NULL);
    hashmap_setb(&map, keys[1], key_len, NULL);
    hashmap_setb(&map, keys[0], key_len, NULL);

    char* k = NULL;
    size_t s, counter = 0;

    while( (k = hashmap_iterb(&map, k, &s)) ){
        assert_int_equal(s, key_len);
        int matched = 0;
        for(size_t i = 0; i != nkeys; ++i){
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
    hashmap_init(&map, 1);

    char* k = NULL;
    size_t s, counter = 0;

    while( (k = hashmap_iterb(&map, k, &s)) ){
        counter++;
    }
    assert_int_equal(counter, 0);
    
    hashmap_uninit(&map);
}

void test_hashmap_set_str(void** state){
    (void)state;
    
    hashmap_t map;
    hashmap_init(&map, 1);

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
    hashmap_init(&map, 1);

    string_t key = string_scoped_lit("key");
    int value = 99;
    hashmap_set(&map, key, &value);

    assert_true(hashmap_has_key(&map, key));

    hashmap_uninit(&map); 
}

void test_hashmap_iter_str(void** state){
    (void)state;

    hashmap_t map;
    hashmap_init(&map, 1);
    size_t nkeys = 3;
    size_t key_len = 4; // not counting null-terminating char
    string_t keys[] = {string_scoped_lit("key1"), string_scoped_lit("key2"), string_scoped_lit("key3")};

    hashmap_set(&map, keys[0], NULL);
    hashmap_set(&map, keys[1], NULL);
    hashmap_set(&map, keys[2], NULL);
    
    string_t k = (string_t){0};
    size_t counter = 0;
    while( (k = hashmap_iter(&map, k)).str ){
        assert_int_equal(k.len, key_len);
        counter++;
    }
    assert_int_equal(counter, nkeys);

    hashmap_uninit(&map); 
}
