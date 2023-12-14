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
	char* key;                       ///< String key
	void* value;                     ///< Data associated with the key
	struct hashmap_entry* next; ///< Linked list for hash collisions
} hashmap_entry_t;

/** @struct hashmap_t
* @brief Hash map data structure. Holds key-value pairs accessed via hashes.
*/
typedef struct hashmap {
	uint32_t size;      ///< total number of buckets
    uint32_t entries;   ///< number of filled buckets
	hashmap_entry_t** table; ///< Hashtable of entries
} hashmap_t;


/** @brief Returns the hash of a given array of bytes.
* The size of the hashmap must be passed as an argument,
* as it will be mod (%) with the hash result.
* @param key can be a pointer to any set of bytes 
* @param length number of bytes in the key
* @param size number of buckets in the hashmap.
*/
uint32_t hashmap_hash_bytes(const char* key, uint32_t length, uint32_t mapsize);

/** @brief Returns the hash of a given key.
* The size of the hashmap must be passed as an argument,
* as it will be mod (%) with the hash result.
* @param key string key
* @param size number of buckets.
*/
uint32_t hashmap_hash(const char* key, uint32_t mapsize);

/** @brief Initialise hashmap via user-managed object.
* Should be deleted using `hashmap_uninit`.
* Returns 0 on success, and 1 on fail.
* @param map Hashmap to initialise
* @param size_hint starting number of buckets
*/
int hashmap_init(hashmap_t* map, uint32_t size_hint);

/** @brief Clears a hashmap and removes all stored data.
* It does not free the pointers to values, as these are managed by the user.
* You must free the values yourself before uninitialising the hashmap.
* You can do this by iterating over the keys and freeing each value in turn.
*/
void hashmap_uninit(hashmap_t* map);

/** @brief Allocates and initialises a hashmap.
* Destroy with `hashmap_destroy`.
* @param size_hint initial number of buckets.
*/
hashmap_t* hashmap_create(uint32_t size_hint);

/** @brief Deallocates a hashmap created with `hashmap_create`.
* It does not free the pointers to values.
* You must free the values yourself before destroying the hashmap.
* You can do this by iterating over the keys and freeing each value in turn.
*/
void hashmap_destroy(hashmap_t* map);

/** @brief Returns 1 if the hashmap contains the given key, and 0 otherwise.
* @param key String key to search for.
*/
int hashmap_has_key(hashmap_t* map, const char* key);


/** @brief Retrieves the data associated with a key.
* If the entry does not exist, NULL is returned.
* @param key String key to search for.
*/
void* hashmap_get(hashmap_t* map, const char* key);

/** @brief Adds or modifies an existing entry using a key.
* Whilst the keys are copied over, the values are not,
* and the user is responsible for providing a pointer to the data
* that lasts for the lifetime of the hashmap.
* Also, if an existing value is overwritten (by using a key that was already in use),
* the pointer to the previous data is lost.
* @param key String key of entry to modify.
* @param value Pointer to user memory to hold on the entry.
*/
hashmap_t* hashmap_set(hashmap_t* map, const char* key, void* value); 

/** @brief Extends the hash table to a size equal to the next prime number
* from its current size.
* 
* This is a CPU intensive operation, as the whole table is rehashed.
* Try this only if you are getting many collisions.
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
* while((key = hashmap_iter_keys(map, key))){
*     printf("%s\n", key);
* }
* ```
* @param key Previous key, or NULL.
*/
char* hashmap_iter_keys(hashmap_t* map, const char* key);


#endif /* HASHMAP_H */
