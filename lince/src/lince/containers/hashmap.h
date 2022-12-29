


/*

`hashmap.h` is an implementation of a dictionary, which is data structure that holds
key-value pairs, using a hashmap.
Hash collisions are handled using linked lists on each bucket.


Example code:

    hashmap_t map;
    hashmap_init(&map, 5); // initial expected size

    int x = 10;
    float y = 0.016;

    hashmap_set(&map, "integer", &x);
    hashmap_set(&map, "floating", &y);

    int a = *(int*)hashmap_get(&map, "integer");
    float b = *(floating*)hashmap_get(&map, "floating");
    
    assert(x == a);
    assert(y == b);

    hashmap_uninit(&map); // does not free stored values

*/


#ifndef HASHMAP_H
#define HASHMAP_h

#include <inttypes.h>


typedef struct hm_entry_container {
	char* key;
	void* value;
	struct hm_entry_container* next;
} hm_entry_t;

typedef struct hashmap_container {
	uint32_t size;      // total number of buckets
    uint32_t entries;   // number of filled buckets
	hm_entry_t **table;
} hashmap_t;


/*
Returns the hash of a given key.
The size of the hashmap must be passed as an argument,
as it will be mod (%) with the hash result.
*/
uint32_t hashmap_hash(const char* key, uint32_t size);

/*
Initialises hashmap via passed pointer.
Should be deleted using `hashmap_uninit`.
Returns 0 on success, and 1 on fail.
*/
int hashmap_init(hashmap_t* map, uint32_t size_hint);

/*
Clears a hashmap and removes all stored data.
It does not free the pointers to values.
You must free the values yourself before uninitialising the hashmap.
You can do this by iterating over the keys and freeing each value in turn.
*/
void hashmap_uninit(hashmap_t* map);

/*
Allocates and initialises a hashmap.
Destroy with `hashmap_destroy`.
*/
hashmap_t* hashmap_create(uint32_t size_hint);

/*
Deallocates a hashmap.
It does not free the pointers to values.
You must free the values yourself before destroying the hashmap.
You can do this by iterating over the keys and freeing each value in turn.
*/
void hashmap_destroy(hashmap_t* map);

/*
Returns 1 if the hashmap contains the given key,
and 0 otherwise.
*/
int hashmap_has_key(hashmap_t* map, const char* key);


/* Retrieves an entry using a key. If the entry does not exist, NULL is returned */
void* hashmap_get(hashmap_t* map, const char* key);

/*
Adds or modifies an existing entry using a key
- Note: whilst the keys are copied over, the values are not,
and the user is responsible for providing a pointer to the data
that lasts for the lifetime of the hashmap.
- Note: if an existing value is overwritten (by using a key that was already in use),
the pointer to the previous data is lost.
*/
hashmap_t* hashmap_set(hashmap_t* map, const char* key, void* value); 

/*
Extends the hash table to a size equal to the next prime number
from its current size.
Note: this is a CPU intensive operation,
as the whole table is rehashed.
Try this only if you are getting many collisions.
*/
hashmap_t* hashmap_resize(hashmap_t* map);

/*
Returns the keys in a hashmap in order.
An existing key must be provided to obtain the next one.
To get the first key, input NULL.
The list of keys ends when the functions returns NULL.
Example:
    char* key = NULL;
    while((key = hashmap_iter_keys(map, key))){
        printf("%s\n", key);
    }
*/
char* hashmap_iter_keys(hashmap_t* map, const char* key);


#endif /* HASHMAP_H */
