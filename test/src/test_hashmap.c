#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/containers/hashmap.h"
#include "test.h"

static void scan_hashmap(hashmap_t* map){
	printf("\n Hashmap size %u and %u entries\n", map->size, map->entries);
	for(uint32_t i = 0; i != map->size; ++i){

		hm_entry_t* entry = map->table[i];
		if(!entry) continue;

		printf("%u) ", i);
		while(entry){
			if(entry->key) printf("<%s>, ", entry->key);
			else printf("<NULL>, ");
			entry = entry->next;
		}
		printf("\n");
	}
}


static int test_hashmap_performance(){

	hashmap_t map;
	hashmap_init(&map, 5);
	int r = 1;

	TEST_CLOCK_START(time);
	int x = 99;
	for(int i = '!'; i != '~'+1; ++i){
		for(int j = '!'; j != '~'+1; ++j){
			char str[] = {i,j,'\0'};
			r = r && hashmap_set(&map, str, &x);
		}
	}
	long int n_op = ('~' - '!') * ('~' - '!');
	TEST_CLOCK_END(time, n_op);
	TEST_ASSERT(r, "Failed to add item to hashmap");

	hashmap_uninit(&map);
	return TEST_PASS;
}

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
		key = hashmap_iter_keys(&map, key);
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

