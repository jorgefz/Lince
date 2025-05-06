
#include "hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* 
 * ----------------
 * Static Functions
 * ----------------
 */


/** Checks whether an input number is prime */
static dast_u64 hashmap_isprime(dast_u64 n) {

    /* Easy cases */ 
    if (n <= 1)  return 0;
    if (n <= 3)  return 1;
    if (n%2 == 0 || n%3 == 0) return 0;

    for (dast_u64 i = 5; i * i <= n; i = i + 6)
        if (n%i == 0 || n%(i+2) == 0){
           return 0;
        }
    return 1;
}

/** Returns the next prime number larger than the given number `n` */
static dast_u64 hashmap_next_prime(dast_u64 n){
    if(n<=1) return 2;
    while(!hashmap_isprime(++n));
    return n;
}


/** Return the hash of a chunk of memory mod'ed with the map size */
static dast_u64 hashmap_get_hash(hashmap_t* map, const void* key, dast_sz keylen){
    return map->hash_fn(key, keylen) % map->size;
}

/** Returns the hashmap element with the given binary key
 * @param map hashmap in which to lookup keys
 * @param bkey binary key
 * @param key_len number of bytes in the key
 * @return hashamp entry associated with the key
*/
static hashmap_entry_t* hashmap_lookupb(hashmap_t* map, const void* bkey, dast_sz key_len) {
    if (!map || !bkey) return dast_null;
    dast_u64 hash = hashmap_get_hash(map, bkey, key_len);
    hashmap_entry_t* entry = map->table[hash];

    while (entry) {
        if (key_len == entry->len && map->eq_fn(bkey, entry->key, key_len)) {
            return entry;
        }
        entry = entry->next;
    }
    return dast_null;
}


/* 
 * ----------------
 * Public Functions
 * ----------------
 */

#define HASHMAP_FNV_64BIT_OFFSET_BASIS ((dast_u64)0xcbf29ce484222325)
#define HASHMAP_FNV_64BIT_PRIME ((dast_u64)0x100000001b3)

/* Computes the hash of a sequence of `len` bytes of `data`
using the FNV1-a hashing algorithm. */
dast_u64 hashmap_FNV1a64_hash(const void* data, dast_sz len){
	const char* p = data;
	dast_u64 hash = HASHMAP_FNV_64BIT_OFFSET_BASIS;
	for(dast_u64 i = 0; i != len; ++i){
		hash = hash ^ p[i];
		hash = hash * HASHMAP_FNV_64BIT_PRIME;
	}
	return hash;
}


/** @brief Initialise hashmap via user-managed object with custom allocator and/or hash function.
 * Should be deleted with `hashmap_uninit`.
 * @param map Hashmap to initialised
 * @param size_hint Starting number of buckets
 * @param alloc Memory allocation functions. If NULL, defaults to 
 * @param hash_fn Hash function. If NULL, defaults to 64-bit FNV-1A.
 * @param eq_fn Key equality function. Needed when hashes collide and keys need to be compared. If NULL, defaults to comparing the raw bytes of the two keys.
*/
hashmap_t* hashmap_init_custom(
	hashmap_t*        map,
	dast_sz           size_hint,
    dast_allocator_t  alloc,
	hashmap_hashfn_t  hash_fn,
    hashmap_eqfn_t    eq_fn
){

    if(!map) return dast_null;
    *map = (hashmap_t){0};

    if (hash_fn) map->hash_fn = hash_fn;
    else         map->hash_fn = hashmap_FNV1a64_hash;

    if (eq_fn)   map->eq_fn   = eq_fn;
    else         map->eq_fn   = dast_memeq;

    if(!alloc.alloc || !alloc.realloc || !alloc.free){
#ifdef DAST_NO_STDLIB
        return dast_null;
#else
        map->alloc = DAST_DEFAULT_ALLOCATOR;
#endif
    } else map->alloc = alloc;

    map->size = (dast_sz)hashmap_next_prime(size_hint);
    map->table = map->alloc.alloc(map->size * sizeof(hashmap_entry_t*));
    if(!map->table){
        return dast_null;
    }
    memset(map->table, 0, map->size * sizeof(hashmap_entry_t*));

    return map;
}


/** @brief Initialise hashmap via user-managed object.
 * Should be deleted using `hashmap_uninit`.
 * @param map Hashmap to initialise
 * @param size_hint starting number of buckets
 * @returns the input map on success, and NULL otherwise
 */
hashmap_t* hashmap_init(hashmap_t* map, dast_sz size_hint){
    return hashmap_init_custom(
        map, size_hint, DAST_DEFAULT_ALLOCATOR, dast_null, dast_null
    );
}

/** @brief Clears a hashmap and removes all stored data.
 * It does not free the pointers to values, as these are managed by the user.
 * You must free the values yourself before uninitialising the hashmap.
 * You can do this by iterating over the keys and freeing each value in turn.
 * @param map hashmap to uninitialise
 */
void hashmap_uninit(hashmap_t* map){
    if(!map || !map->table) return;

    for(dast_sz i=0; i!=map->size; ++i){
        hashmap_entry_t* entry = map->table[i];
        hashmap_entry_t* next;
        while(entry){
            next = entry->next;
            map->alloc.free(entry->key);
            map->alloc.free(entry);
            entry = next;
        }
    }
    map->alloc.free(map->table);
    *map = (hashmap_t){0};
}


/** @brief Checks if a map has a given key
 * @param map initialised hashmap
 * @param bkey key to find, can be any set of bytes
 * @param key_len number of bytes in the key
 * @returns `dast_true` if key exists in the map, and `dast_false` otherwise
 */
dast_bool hashmap_has_keyb(hashmap_t* map, const void* bkey, dast_sz key_len) {
    if (!bkey) return 0;
    return (hashmap_lookupb(map, bkey, key_len) != dast_null);
}

/** @brief Checks if a map has a given string key
 * @param map initialised hashmap
 * @param key string key
 * @returns `dast_true` if key exists in the map, and `dast_false` otherwise
 */
dast_bool hashmap_has_key(hashmap_t* map, string_t key){
    if (!key.str) return dast_false;
    return hashmap_has_keyb(map, key.str, key.len + 1); /* Include null-terminating char */
}

/** @brief Retrieves the data associated with a key.
 * @param hashmap to query
 * @param bkey key to search for, which can be any set of bytes
 * @param key_len number of bytes in the key
 * @returns map element associated to the input key, or NULL if the key does not exist
 */
void* hashmap_getb(hashmap_t* map, const void* bkey, dast_sz key_len) {
    hashmap_entry_t* entry = hashmap_lookupb(map, bkey, key_len);
    if (!entry) return dast_null;
    return entry->value;
}

/** @brief Retrieves the data associated with a key.
 * @param hashmap to query
 * @param bkey key to search for, which can be any set of bytes
 * @returns map element associated to the input key, or NULL if the key does not exist.
 * @note The function may also return dast_null if the key exists but it is mapped to a NULL value.
 */
void* hashmap_get(hashmap_t* map, string_t key){
    if (!key.str) return dast_null;
    return hashmap_getb(map, key.str, key.len + 1); /* Include null-terminating char */
}

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
hashmap_t* hashmap_setb(hashmap_t* map, const void* bkey, dast_sz key_len, void* value) {
    if (!map || !bkey) return dast_null;

    hashmap_entry_t* entry = hashmap_lookupb(map, bkey, key_len);
    dast_u64 hash = map->hash_fn(bkey, key_len) % map->size;

    while (entry) {
        if (key_len == entry->len && map->eq_fn(bkey, entry->key, key_len)) {
            entry->value = value;
            return map;
        }
        entry = entry->next;
    }

    /* No matching key found */
    entry = map->alloc.alloc(sizeof(hashmap_entry_t));
    if (!entry) return dast_null;

    memset(entry, 0, sizeof(hashmap_entry_t));
    entry->key = map->alloc.alloc(key_len);
    if (!entry->key) {
        map->alloc.free(entry);
        return dast_null;
    }

    memcpy(entry->key, bkey, key_len);
    entry->len = key_len;
    entry->value = value;
    entry->next = map->table[hash];
    map->table[hash] = entry;

    /* Extend if necessary */
    map->entries++;
    if (map->entries * HASHMAP_LOADING_FACTOR >= map->size) {
        hashmap_resize(map);
    }
    return map;
}

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
hashmap_t* hashmap_set(hashmap_t* map, string_t key, void* value){
    if (!key.str) return dast_null;
    return hashmap_setb(map, key.str, key.len + 1, value); /* Include null-terminating char */
}

/** @brief Extends the hash table to a size equal to the next prime number from its current size.
 * @param map hashmap to extend
 * @returns the input map if successful, and NULL otherwise
 * @note This is a CPU intensive operation, as the whole table is rehashed.
 * The table should resize itself automatically when the number of keys
 * reaches some fraction of the number of buckets.
 */
hashmap_t* hashmap_resize(hashmap_t* map) {
    if (!map) return dast_null;

    dast_sz new_size = map->entries * HASHMAP_LOADING_FACTOR;
    hashmap_t new_map;
    hashmap_init_custom(&new_map, new_size, map->alloc, map->hash_fn, map->eq_fn);
    hashmap_entry_t* entry;
    dast_sz i;

    /* Rehash table */
    for (i = 0; i != map->size; ++i) {
        entry = map->table[i];
        while (entry) {
            hashmap_setb(&new_map, entry->key, entry->len, entry->value);
            entry = entry->next;
        }
    }

    hashmap_uninit(map);
    memmove(map, &new_map, sizeof(hashmap_t));
    return map;
}

/** @brief Returns the next key in a hashmap.
 * @param bkey Previous key, which can be any set of bytes. To start iterating, input NULL.
 * @param key_len number of bytes in the key. Must point to valid memory.
 * @returns the next key in the hashmap, with it length stored in the input `key_len`.
 * @note When the functions returns NULL, there are no more keys to fetch.
 * Example:
 * 	```c
 * 	char* key = NULL;
 * 	dast_u64 len = 0;
 * 	do{
 * 		key = hashmap_iterb(map, key, &len);
 * 	} while(key);
 * 	```
 */
void* hashmap_iterb(hashmap_t* map, const char* bkey, dast_sz* key_len) {
    if (!map || !map->table) return dast_null;

    hashmap_entry_t* entry = dast_null;
    dast_u64 hash;

    /* Search from the beginning of the hash table */
    if (!bkey) {
        for (dast_sz i = 0; i != map->size; ++i) {
            if (map->table[i]) {
                if (key_len) {
                    *key_len = map->table[i]->len;
                }
                return map->table[i]->key;
            }
        }
        return dast_null;
    }

    /* Fetch the next key with the same hash (in a linked list) */
    entry = hashmap_lookupb(map, bkey, *key_len);
    if(!entry) return dast_null; /* Key provided does not exist in the map */
    if (entry->next) {
        if (key_len) {
            *key_len = entry->next->len;
        }
        return entry->next->key;
    }

    /* Fetch the key in the table (with a different hash) */
    hash = map->hash_fn(bkey, *key_len) % map->size;
    for (dast_u64 i = hash + 1; i != map->size; ++i) {
        if (map->table[i]) {
            if (key_len) {
                *key_len = map->table[i]->len;
            }
            return map->table[i]->key;
        }
    }
    return dast_null;
}

/** @brief Returns the next key in a hashmap.
 * @param key Previous string key. To start iterating, input empty string (where `str` field is NULL).
 * @returns the next key in the hashmap.
 * @note When the functions returns NULL, there are no more keys to fetch.
 * @warning Assumess all keys are null-terminating strings.
 * Example:
 * 	```c
 * 	string_t key = (string_t){0};
 *  while( hashmap_iter_keys(map, &key) ){
 *      ...
 *  }
 * 	```
*/
string_t* hashmap_iter(hashmap_t* map, string_t* key){
    if(!key) return dast_null;

    key->len++; /* Account for null-terminating char when iterating */
    key->str = hashmap_iterb(map, key->str, &key->len);
    key->len--; /* Ignore null-terminating char in string_t* */

    if(key->str) return key;
    return dast_null;
}
