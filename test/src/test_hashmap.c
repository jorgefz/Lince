#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/containers/hashmap.h"
#include "benchmark.h"



void test_hashmap(void** state){
	(void)state;

	// Initialise
	hashmap_t map;
	hashmap_init(&map, 5);
	assert_non_null(map.table);
	assert_true(map.size == 7);
	
	// Initialise with size hint zero
	hashmap_t map0;
	hashmap_init(&map0, 0);
	assert_non_null(map0.table);
	assert_true(map0.size == 2);
	hashmap_uninit(&map0);

	// Hashing function
	uint32_t map_size = 10;
	uint32_t hash1 = hashmap_hash("key", map_size);
	uint32_t hash2 = hashmap_hashb("key", 4, map_size);
	assert_int_equal(hash1, hash2);

	// Set values
	int r = 1, x = 10, y = 20, z = 30;
	r = r && hashmap_set(&map, "x", &x);
	r = r && hashmap_set(&map, "y", &y);
	r = r && hashmap_set(&map, "z", &z);
	assert_true(r);
	
	// Retrieve values
	int *rx, *ry, *rz, *none;
	rx = hashmap_get(&map, "x");
	ry = hashmap_get(&map, "y");
	rz = hashmap_get(&map, "z");
	none = hashmap_get(&map, "n");
	
	assert_true(rx && ry && rz && !none);
	assert_true(*rx == x && *ry == y && *rz == z);
	assert_true(map.entries == 3);
	
	// Iterate over keys
	char* key = NULL;
	int key_count = 0;
	do{
		key = hashmap_iter(&map, key);
		key_count++;
	} while(key);
	key_count--;
	assert_true(key_count==3);
	hashmap_uninit(&map);

	// Allocate and destroy
	hashmap_t* p = hashmap_create(2);
	assert_non_null(p);
	hashmap_destroy(p);

}


void test_hashmap_byte_key(void** state) {
	(void)state;

	// Initialise
	hashmap_t map;
	hashmap_init(&map, 5);
	assert_non_null(map.table);
	assert_true(map.size == 7);

	// Initialise with size hint zero
	hashmap_t map0;
	hashmap_init(&map0, 0);
	assert_non_null(map0.table);
	assert_true(map0.size == 2);
	hashmap_uninit(&map0);

	// Hashing function a string
	uint32_t some_map_size = 100; // large enough to avoid collisions
	uint32_t hash1 = hashmap_hash("key", some_map_size);
	uint32_t hash2 = hashmap_hashb("key", 4, some_map_size);
	assert_int_equal(hash1, hash2);

	// Hashing function with other plain-old-data
	int key_int1 = 10, key_int2 = 11;
	uint32_t hash_int1 = hashmap_hashb(&key_int1, sizeof(int), some_map_size);
	uint32_t hash_int2 = hashmap_hashb(&key_int2, sizeof(int), some_map_size);
	assert_int_not_equal(hash_int1, hash_int2);

	float key_float1 = 1e-3f, key_float2 = 1e5f;
	uint32_t hash_float1 = hashmap_hashb(&key_float1, sizeof(float), some_map_size);
	uint32_t hash_float2 = hashmap_hashb(&key_float2, sizeof(float), some_map_size);
	assert_int_not_equal(hash_float1, hash_float2);

	// Set values
	int r = 1, x = 10, y = 20, z = 30;
	r = r && hashmap_setb(&map, &x, sizeof(int), &x);
	r = r && hashmap_setb(&map, &y, sizeof(int), &y);
	r = r && hashmap_setb(&map, &z, sizeof(int), &z);
	assert_true(r);

	// Retrieve values
	int* rx = hashmap_getb(&map, &x, sizeof(int));
	assert_non_null(rx);
	assert_int_equal(*rx, x);

	int* ry = hashmap_getb(&map, &y, sizeof(int));
	assert_non_null(ry);
	assert_int_equal(*ry, y);

	int* rz = hashmap_getb(&map, &z, sizeof(int));
	assert_non_null(rz);
	assert_int_equal(*rz, z);

	int* none = hashmap_getb(&map, "n", 1);
	assert_null(none);

	assert_true(map.entries == 3);
	
	// Iterate over keys
	char* key = NULL;
	uint32_t len = 0;
	int key_count = 0;
	do {
		key = hashmap_iterb(&map, key, len, &len);
		key_count++;
	} while (key);
	key_count--;
	assert_int_equal(key_count, 3);
	assert_int_equal(len, sizeof(int));

	hashmap_uninit(&map);

	// Allocate and destroy
	hashmap_t* p = hashmap_create(2);
	assert_non_null(p);
	hashmap_destroy(p);

}


void benchmark_hashmap() {

	int value = 99;
	const int iter = 100000;
	size_t r = 1;

	hashmap_t map;
	hashmap_init(&map, iter);

	for (int i = 0; i != iter; ++i) {
		hashmap_setb(&map, &i, sizeof(int), &value);
	}

	printf("[BENCHMARK] hashmap_getb\n");

	BENCHMARK_LOOP(int, i, iter) {
		r = r && hashmap_getb(&map, &i, sizeof(int));
	} BENCHMARK_END(int, i, iter);

	hashmap_uninit(&map);
	return 0;
}