/** @file hashmap.h
* `hashmap.h` is an implementation of a dictionary, which is data structure that holds
* key-value pairs, using a hashmap.
* Hash collisions are handled using linked lists on each bucket.
* 
* Example code:
* ```c
*     hashmap_t map;
*     hashmap_init(&map, 5); // initial expected size
* 
*     int x = 10;
*     float y = 0.016;
* 
*     hashmap_set(&map, string_scoped_lit("int"),   &x);
*     hashmap_set(&map, string_scoped_lit("float"), &y);
* 
*     int a = *(int*)hashmap_get(&map, string_scoped_lit("integer"));
*     float b = *(float*)hashmap_get(&map, string_scoped_lit("floating"));
*     
*     assert(x == a);
*     assert(y == b);
* 
*     hashmap_uninit(&map); // does not free stored values
* ```
*/


#ifndef HASHMAP_H
#define HASHMAP_H

#include <inttypes.h>
#include "str.h"

#ifdef HASHMAP_HASH_32BIT
	typedef uint32_t hashmap_hash_t; ///< @typedef Type of 64-bit hashed value
#else
	typedef uint64_t hashmap_hash_t; ///< @typedef Type of 32-bit hashed value
#endif

/** @typedef Type for hashing function */
typedef hashmap_hash_t (*hashmap_hashfn_t)(const void* data, size_t len);

/** @typedef Type for allocator function */
typedef void* (*hashmap_alloc_t)(size_t size);

/** @typedef Type for reallocator function */
typedef void* (*hashmap_realloc_t)(void* block, size_t new_size);

/** @typedef Type for deallocator function */
typedef void (*hashmap_free_t)(void* block);


/** @struct hashmap_entry
 * @brief Hashmap entry. Holds a key-value pair.
 */
typedef struct hashmap_entry {
	char*    key;               ///< Key (may be string or binary)
	uint64_t len;				///< Number of bytes in the key	
	void*    value;             ///< Data associated with the key
	struct hashmap_entry* next; ///< Linked list for hash collisions
} hashmap_entry_t;

/** @struct hashmap_t
 * @brief Hash map data structure. Holds key-value pairs accessed via hashes.
 */
typedef struct hashmap {
	uint64_t          size;        ///< Total number of buckets
    uint64_t          entries;     ///< Number of filled buckets
	hashmap_entry_t** table;       ///< Hash table of entries

	hashmap_hashfn_t  hash_fn;     ///< Hashing function
	hashmap_alloc_t   alloc_fn;    ///< Memory allocation function
	hashmap_realloc_t realloc_fn;  ///< Memory reallocation function
	hashmap_free_t    free_fn;     ///< Memory deallocation function
} hashmap_t;


/** @brief FNV1-a 64-bit hashing algorithm */
uint64_t hashmap_FNV1a64_hash(const void* data, uint64_t len);


// Set default hash function
// void hashmap_set_default_hash_fn();


/** @brief Initialise hashmap via user-managed object.
 * Should be deleted using `hashmap_uninit`.
 * @param map Hashmap to initialise
 * @param size_hint starting number of buckets
 * @returns the input map on success, and NULL otherwise
 */
hashmap_t* hashmap_init(hashmap_t* map, uint64_t size_hint);

/** @brief Initialise hashmap via user-managed object with custom allocator and/or hash function.
 * Should be deleted with `hashmap_uninit`.
 * @param map Hashmap to initialised
 * @param size_hint Starting number of buckets
 * @param hash_fn Hash function
 * @param alloc_fn Allocation function
 * @param realloc_fn Reallocation function
 * @param free_fn Deallocation function
*/
hashmap_t* hashmap_init_custom(
	hashmap_t*        map,
	uint64_t          size_hint,
	hashmap_hashfn_t  hash_fn,
	hashmap_alloc_t   alloc_fn,
	hashmap_realloc_t realloc_fn,
	hashmap_free_t    free_fn
);


/** @brief Clears a hashmap and removes all stored data.
 * It does not free the pointers to values, as these are managed by the user.
 * You must free the values yourself before uninitialising the hashmap.
 * You can do this by iterating over the keys and freeing each value in turn.
 * @param map hashmap to uninitialise
 */
void hashmap_uninit(hashmap_t* map);

/** @brief Checks if a map has a given key
 * @param map initialised hashmap
 * @param bkey key to find, can be any set of bytes
 * @param key_len number of bytes in the key
 * @returns 1 if key exists in the map, and 0 otherwise
 */
int hashmap_has_keyb(hashmap_t* map, const void* bkey, uint64_t key_len);

/** @brief Checks if a map has a given string key
 * @param map initialised hashmap
 * @param key string key
 * @returns 1 if key exists in the map, and 0 otherwise
 */
int hashmap_has_key(hashmap_t* map, string_t key);

/** @brief Retrieves the data associated with a key.
 * @param hashmap to query
 * @param bkey key to search for, which can be any set of bytes
 * @param key_len number of bytes in the key
 * @returns map element associated to the input key, or NULL if the key does not exist
 */
void* hashmap_getb(hashmap_t* map, const void* bkey, uint64_t key_len);

/** @brief Retrieves the data associated with a key.
 * @param hashmap to query
 * @param bkey key to search for, which can be any set of bytes
 * @returns map element associated to the input key, or NULL if the key does not exist.
 * @note The function may also return NULL if the key exists but it is mapped to a NULL value.
 */
void* hashmap_get(hashmap_t* map, string_t key);

/** @brief Adds a new key-value pair to a hashmap. If the key already exists, the value is replaced.
 * @param map hashmap to which to insert value
 * @param bkey key to insert, can be any set of bytes
 * @param key_len number of bytes in the key
 * @param value pointer to value to insert
 * @returns pointer to map if insert is successful, or NULL otherwise
 * @note the value is not copied over, only a pointer to it is stored.
 * The user is responsible for ensuring that the lifetime of the memory where the value is stored
 * lasts for the lifetime of the corresponding hashmap key-value pair.
 * If this function is used to replace a value with the same key, the previous value pointer is dropped!!
 * Moreover, unlike the value, a copy of the key IS stored.
 */
hashmap_t* hashmap_setb(hashmap_t* map, const void* bkey, uint64_t key_len, void* value);

/** @brief Adds a new key-value pair to a hashmap. If the key already exists, the value is replaced.
 * @param map hashmap to which to insert value
 * @param key string key
 * @param value pointer to value to insert
 * @returns pointer to map if insert is successful, or NULL otherwise
 * @note if the value is not copied over, only a pointer to it is stored.
 * The user is responsible for ensuring that the lifetime of the memory where the value is stored
 * lasts for the lifetime of the corresponding hashmap key-value pair.
 * If this function is used to replace a value with the same key, the previous value pointer is dropped.
 * Moreover, unlike the value, a copy of the string key IS stored.
 */
hashmap_t* hashmap_set(hashmap_t* map, string_t key, void* value); 

/** @brief Extends the hash table to a size equal to the next prime number from its current size.
 * @param map hashmap to extend
 * @returns mthe input map if successful, and NULL otherwise
 * @note This is a CPU intensive operation, as the whole table is rehashed.
 * The table should resize itself automatically when the number of keys
 * reaches some fraction of the number of buckets.
 */
hashmap_t* hashmap_resize(hashmap_t* map);

/** @brief Returns the next key in a hashmap.
 * @param bkey Previous key, which can be any set of bytes. To start iterating, input NULL.
 * @param key_len number of bytes in the key. Must point to valid memory.
 * @returns the next key in the hashmap, with it length stored in the input `key_len`.
 * @note When the functions returns NULL, there are no more keys to fetch.
 * Example:
 * 	```c
 * 	char* key = NULL;
 * 	uint64_t len = 0;
 * 	do{
 * 		key = hashmap_iterb(map, key, &len);
 * 	} while(key);
 * 	```
 */
void* hashmap_iterb(hashmap_t* map, const char* bkey, uint64_t* key_len);

/** @brief Returns the next key in a hashmap.
 * @param key Previous string key. To start iterating, input empty string (where `str` field is NULL).
 * @returns the next key in the hashmap.
 * @note When the functions returns NULL, there are no more keys to fetch.
 * Example:
 * 	```c
 * 	string_t key = (string_t){0};
 * 	do{
 * 		key = hashmap_iter_keysb(map, key);
 * 	} while(key);
 * 	```
*/
string_t hashmap_iter(hashmap_t* map, string_t key);


#endif /* HASHMAP_H */
