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
*     hashmap_set(&map, "integer", &x);
*     hashmap_set(&map, "floating", &y);
* 
*     int a = *(int*)hashmap_get(&map, "integer");
*     float b = *(floating*)hashmap_get(&map, "floating");
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

/** @struct hashmap_entry
* @brief Hashmap entry. Holds a key-value pair.
*/
typedef struct hashmap_entry {
	char*    key;               ///< String key
	uint32_t len;				///< Length of key	
	void*    value;             ///< Data associated with the key
	struct hashmap_entry* next; ///< Linked list for hash collisions
} hashmap_entry_t;

/** @struct hashmap_t
* @brief Hash map data structure. Holds key-value pairs accessed via hashes.
*/
typedef struct hashmap {
	uint32_t size;           ///< total number of buckets
    uint32_t entries;        ///< number of filled buckets
	hashmap_entry_t** table; ///< Hashtable of entries
} hashmap_t;


/** @brief Returns the hash of a given number of bytes.
* The size of the hashmap must be passed as an argument,
* as it will be mod (%) with the hash result.
* @param key key to hash, can be any set of bytes 
* @param key_length number of bytes in the key
* @param map_size number of buckets in the hashmap.
* @returns hash of the inout key
*/
uint32_t hashmap_hashb(const void* key, uint32_t key_length, uint32_t map_size);

/** @brief Returns the hash of a given string key.
* The size of the hashmap must be passed as an argument,
* as it will be mod (%) with the hash result.
* @param key key, must be null-terminated string
* @param map_size number of buckets.
* * @returns hash of the inout key
*/
uint32_t hashmap_hash(const char* key, uint32_t map_size);

/** @brief Initialise hashmap via user-managed object.
* Should be deleted using `hashmap_uninit`.
* @param map Hashmap to initialise
* @param size_hint starting number of buckets
* @return 0 on success, and 1 on fail.
*/
int hashmap_init(hashmap_t* map, uint32_t size_hint);

/** @brief Clears a hashmap and removes all stored data.
* It does not free the pointers to values, as these are managed by the user.
* You must free the values yourself before uninitialising the hashmap.
* You can do this by iterating over the keys and freeing each value in turn.
* @param map hashmap to uninitialise
*/
void hashmap_uninit(hashmap_t* map);

/** @brief Allocates and initialises a hashmap.
* Destroy with `hashmap_destroy`.
* @param size_hint initial number of buckets.
* @returns pointer to new hashmap
*/
hashmap_t* hashmap_create(uint32_t size_hint);

/** @brief Deallocates a hashmap created with `hashmap_create`.
* It does not free the pointers to values.
* You must free the values yourself before destroying the hashmap.
* You can do this by iterating over the keys and freeing each value in turn.
* @param map hasmap to delete
*/
void hashmap_destroy(hashmap_t* map);

/* @brief Checks if a map has a given key
* @param map initialised hashmap
* @param key key to find, can be any set of bytes
* @param key_length number of bytes in the key
* @returns 1 if key exists in the map, and 0 otherwise
*/
int hashmap_has_keyb(hashmap_t* map, const void* key, uint32_t key_length);


/* @brief Checks if a map has a given string key
* @param map initialised hashmap
* @param key key to find, must be null-terminated string
* @returns 1 if key exists in the map, and 0 otherwise
*/
int hashmap_has_key(hashmap_t* map, const char* key);


/** @brief Retrieves the data associated with a key.
* @param hashmap to query
* @param key key to search for, which can be any set of bytes
* @param key_lentgh number of bytes in the key
* @returns map element associated to the input key, or NULL if the key does not exist
*/
void* hashmap_getb(hashmap_t* map, const void* key, uint32_t key_length);


/** @brief Retrieves the data associated with a key.
* @param hashmap to query
* @param key key to search for, must be null-terminated string
* @param key_lentgh number of bytes in the key
* @returns map element associated to the input key, or NULL if the key does not exist
*/
void* hashmap_get(hashmap_t* map, const char* key);


/** @brief Adds a new key-value pair to a hashmap. If the key already exists, the value is replaced.
* @param map hashmap to which to insert value
* @param key key to insert, can be any set of bytes
* @param key_length number of bytes in the key
* @param value pointer to value to insert
* @returns pointer to map if insert is successful, or NULL otherwise
* @note if the value is not copied over, only a pointer to it is stored.
* The user is responsible for ensuring that the lifetime of the memory where the value is stored
* lasts for the lifetime of the corresponding hashmap key-value pair.
* If this function is used to replace a value with the same key, the previous value pointer is dropped.
*/
hashmap_t* hashmap_setb(hashmap_t* map, const void* key, uint32_t key_length, void* value);


/** @brief Adds a new key-value pair to a hashmap. If the key already exists, the value is replaced.
* @param map hashmap to which to insert value
* @param key key to insert, must be a null-terminated string
* @param value pointer to value to insert
* @returns pointer to map if insert is successful, or NULL otherwise
* @note if the value is not copied over, only a pointer to it is stored.
* The user is responsible for ensuring that the lifetime of the memory where the value is stored
* lasts for the lifetime of the corresponding hashmap key-value pair.
* If this function is used to replace a value with the same key, the previous value pointer is dropped.
*/
hashmap_t* hashmap_set(hashmap_t* map, const char* key, void* value); 


/** @brief Extends the hash table to a size equal to the next prime number from its current size.
* @param map hashmap to extend
* @returns mthe input map if successful, and NULL otherwise
* @note This is a CPU intensive operation, as the whole table is rehashed. Try this only if you are getting many collisions.
*/
hashmap_t* hashmap_resize(hashmap_t* map);


/** @brief Returns the keys in a hashmap in order.
*
* An existing key must be provided to obtain the next one.
* To get the first key, input NULL.
* The list of keys ends when the functions returns NULL.
* Example:
* ```c
* char* key = NULL;
* uint32_t len = 0, next_len = 0;
* do{
*	key = hashmap_iter_keysb(map, key, len, &next_len);
*	len = next_len;
* } while(key);
* ```
* @param key Previous key, which can be any set of bytes. To start iterating, input NULL.
* @param key_length number of bytes in the key. To start iterating, input a value of 0.
* @param next_key_length number of bytes in the next key
* @returns the next key in the hashmap
*/
void* hashmap_iterb(hashmap_t* map, const char* key, uint32_t key_length, uint32_t* next_key_length);


/** @brief Returns the keys in a hashmap in order.
*
* An existing key must be provided to obtain the next one.
* To get the first key, input NULL.
* The list of keys ends when the functions returns NULL.
* Example:
* ```c
* char* key = NULL;
* do{
*	key = hashmap_iter_keysb(map, key);
* } while(key);
* ```
* @param key Previous key, which must be a null-terminated string. To start iterating, input NULL.
* @returns the next key in the hashmap
*/
char* hashmap_iter(hashmap_t* map, const char* key);


#endif /* HASHMAP_H */
