#include "lince/containers/hashmap.h"

#include "test.h"
#include "targets.h"

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


int test_hashmap_performance(){

	hashmap_t map = hashmap_create(5);
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

	// scan_hashmap(&map);
	hashmap_free(&map);
	return TEST_PASS;
}

int test_hashmap(){

	hashmap_t map = hashmap_create(5);
	TEST_ASSERT(map.table && map.size==7, "Failed to create hashmap");
	
	int r = 1, x = 10, y = 20, z = 30;
	r = r && hashmap_set(&map, "x", &x);
	r = r && hashmap_set(&map, "y", &y);
	r = r && hashmap_set(&map, "z", &z);
	TEST_ASSERT(r, "Failed to insert values to hashmap");
	
	int *rx, *ry, *rz, *none;
	rx = hashmap_get(&map, "x");
	ry = hashmap_get(&map, "y");
	rz = hashmap_get(&map, "z");
	none = hashmap_get(&map, "n");
	
	TEST_ASSERT(rx && ry && rz && !none, "Failed to retrieve values from hashmap");
	TEST_ASSERT(*rx == x && *ry == y && *rz == z && map.entries == 3,
		"Failed to retrieve values from hashmap");
	
	char* key = NULL;
	int key_count = 0;
	do{
		key = hashmap_iter_keys(&map, key);
		key_count++;
	} while(key);
	key_count--;
	TEST_ASSERT(key_count==3, "Key iteration failed, unexpected number of keys");

	// scan_hashmap(&map);
	hashmap_free(&map);

	return TEST_PASS;
}

