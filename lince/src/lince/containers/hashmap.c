
#include "hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHMAP_LOADING_FACTOR 2

/*
    static functions
*/

// Calculates whether a given number `n` is prime
static int isprime(int n) {
    enum {false, true};

    // Easy cases
    if (n <= 1)  return false;
    if (n <= 3)  return true;
    if (n%2 == 0 || n%3 == 0) return false;

    for (int i=5; i*i<=n; i=i+6)
        if (n%i == 0 || n%(i+2) == 0)
           return false;
   
    return true;
}

// Returns the next prime number larger than the given number `n`
static int next_prime(int n){
    if(n<=1) return 2;
    while(!isprime(++n));
    return n;
}

// Returns 1 if data at two locations are equal, and zero otherwise
static int memeq(const void* b1, const void* b2, uint32_t s1, uint32_t s2) {
    return (b1 && b2) && (s1 == s2) && ((b1 == b2) || (memcmp(b1, b2, s1) == 0));
}


// Returns the hashmap element with the given key of arbitrary type
static hashmap_entry_t* hashmap_lookupb(hashmap_t* map, const void* key_bytes, uint32_t key_length) {
    if (!map || !key_bytes) return NULL;
    uint32_t hash = hashmap_hashb(key_bytes, key_length, map->size);
    hashmap_entry_t* entry = map->table[hash];

    while (entry) {
        if (memeq(key_bytes, entry->key, key_length, entry->len)) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}


// Returns the hashmap element with the given string key
static hashmap_entry_t* hashmap_lookup(hashmap_t* map, const char* key){
    if (!key) return NULL;
    return hashmap_lookupb(map, key, (uint32_t)strlen(key) + 1);
}



uint32_t hashmap_hashb(const void* key_bytes, uint32_t key_length, uint32_t map_size) {
    // Using 'one-at-a-time' hashing function by Bob Jenkins
    // https://en.wikipedia.org/wiki/Jenkins_hash_function
    size_t i = 0;
    uint32_t hash = 0;
    const char* key = key_bytes;
    while (i != key_length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash % map_size;
}

uint32_t hashmap_hash(const char* key, uint32_t map_size) {
    if (!key) return 0;
    return hashmap_hashb(key, (uint32_t)strlen(key) + 1, map_size);
}


hashmap_t* hashmap_init(hashmap_t* map, uint32_t size_hint){
    *map = (hashmap_t){0};
    map->size = next_prime(size_hint);
    map->table = calloc(map->size, sizeof(hashmap_entry_t*));
    if(!map->table) return NULL;
    return map;
}

/* Clears and deallocates a hashmap */
void hashmap_uninit(hashmap_t* map){
    if(!map || !map->table) return;

    for(uint32_t i=0; i!=map->size; ++i){
        hashmap_entry_t* entry = map->table[i], *next;
        while(entry){
            next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(map->table);
    *map = (hashmap_t){0};
}

hashmap_t* hashmap_create(uint32_t size_hint){
    hashmap_t* map = malloc(sizeof(hashmap_t));
    if(!map) return NULL;
    hashmap_init(map, size_hint);
    return map;
}

void hashmap_destroy(hashmap_t* map){
    if(!map) return;
    hashmap_uninit(map);
    free(map);
}


/*
Returns 1 if the hashmap contains the given byte key,
and 0 otherwise.
*/
int hashmap_has_keyb(hashmap_t* map, const void* key, uint32_t key_length) {
    if (!key) return 0;
    return (hashmap_lookupb(map, key, key_length) != NULL);
}


/*
Returns 1 if the hashmap contains the given key,
and 0 otherwise.
*/
int hashmap_has_key(hashmap_t* map, const char* key){
    return hashmap_has_keyb(map, key, (uint32_t)strlen(key) + 1);
}


/* Retrieves an entry using a byte key. If the entry does not exist, NULL is returned */
void* hashmap_getb(hashmap_t* map, const void* key, uint32_t key_length) {
    hashmap_entry_t* entry = hashmap_lookupb(map, key, key_length);
    if (!entry) return NULL;
    return entry->value;
}


/* Retrieves an entry using a key. If the entry does not exist, NULL is returned */
void* hashmap_get(hashmap_t* map, const char* key){
    if (!key) return NULL;
    return hashmap_getb(map, key, (uint32_t)(strlen(key) + 1));
}


hashmap_t* hashmap_setb(hashmap_t* map, const void* key, uint32_t key_length, void* value) {
    if (!map || !key) return NULL;

    hashmap_entry_t* entry = hashmap_lookupb(map, key, key_length);
    uint32_t hash = hashmap_hashb(key, key_length, map->size);

    while (entry) {
        if (memeq(key, entry->key, key_length, entry->len)) {
            entry->value = value;
            return map;
        }
        entry = entry->next;
    }

    // No matching key found
    entry = calloc(1, sizeof(hashmap_entry_t));
    if (!entry) return NULL;

    entry->key = malloc(key_length);
    if (!entry->key) {
        free(entry);
        return NULL;
    }

    memcpy(entry->key, key, key_length);
    entry->len = key_length;
    entry->value = value;
    entry->next = map->table[hash];
    map->table[hash] = entry;

    // Extend if necessary
    map->entries++;
    if (map->entries * HASHMAP_LOADING_FACTOR >= map->size) {
        hashmap_resize(map);
    }
    return map;
}


hashmap_t* hashmap_set(hashmap_t* map, const char* key, void* value){
    if (!key) return NULL;
    return hashmap_setb(map, key, (uint32_t)strlen(key) + 1, value);
}


hashmap_t* hashmap_resize(hashmap_t* map) {
    if (!map) return NULL;

    uint32_t new_size = map->entries * HASHMAP_LOADING_FACTOR;
    hashmap_t new_map;
    hashmap_init(&new_map, new_size);
    hashmap_entry_t* entry;
    uint32_t i;

    // Rehash table
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


void* hashmap_iterb(hashmap_t* map, const char* key, uint32_t key_length, uint32_t* next_length) {
    if (!map || !map->table) return NULL;

    hashmap_entry_t* entry = NULL;
    uint32_t hash;

    // Search from the beginning of the hash table
    if (!key || !(entry = hashmap_lookupb(map, key, key_length))) {
        for (uint32_t i = 0; i != map->size; ++i) {
            if (map->table[i]) {
                if (next_length) {
                    *next_length = map->table[i]->len;
                }
                return map->table[i]->key;
            }
        }
        return NULL;
    }

    // Search from hash of given key
    if (entry->next) {
        if (next_length) {
            *next_length = entry->next->len;
        }
        return entry->next->key;
    }

    hash = hashmap_hashb(key, key_length, map->size);
    for (uint32_t i = hash + 1; i != map->size; ++i) {
        if (map->table[i]) {
            if (next_length) {
                *next_length = map->table[i]->len;
            }
            return map->table[i]->key;
        }
    }
    return NULL;
}


char* hashmap_iter(hashmap_t* map, const char* key){
    size_t sz = key ? (strlen(key) + 1) : 0;
    return hashmap_iterb(map, key, (uint32_t)sz, NULL);
}
