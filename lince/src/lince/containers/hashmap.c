
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

// Evaluates to true if data at two locations are equal
static int memeq(const void* b1, const void* b2, uint32_t s1, uint32_t s2) {
    return (b1 && b2) && (s1 == s2) && ((b1 == b2) || (memcmp(b1, b2, s1) == 0));
}


// Evaluates to true (1) if both strings are equal
static int streq(const char* s1, const char* s2){
    return (s1 && s2) && ((s1 == s2) || (strcmp(s1,s2) == 0));
}


// Returns the hashmap element with the given key of arbitrary type
static hashmap_entry_t* hashmap_lookup_b(hashmap_t* map, const void* key_bytes, uint32_t key_length) {
    if (!map || !key_bytes) return NULL;
    uint32_t hash = hashmap_hash_b(key_bytes, key_length, map->size);
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
    if(!map || !key) return NULL;
    uint32_t hash = hashmap_hash(key, map->size);
    hashmap_entry_t* entry = map->table[hash];

    while(entry){
        if (streq(key, entry->key)){
            return entry;
        }
        entry = entry->next;
    }
	return NULL;
}



/*
    API definitions
*/

uint32_t hashmap_hash_b(const void* key_bytes, uint32_t key_length, uint32_t map_size) {
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
    return hashmap_hash_b(key, strlen(key)+1, map_size);
}

/*
Initialises hashmap via passed pointer.
Should be deleted using `hashmap_uninit`.
*/
int hashmap_init(hashmap_t* map, uint32_t size_hint){
    *map = (hashmap_t){0};
    map->size = next_prime(size_hint);
    map->table = calloc(map->size, sizeof(hashmap_entry_t*));
    if(!map->table) return 1;
    return 0;
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
int hashmap_has_key_b(hashmap_t* map, const void* key_bytes, uint32_t key_length) {
    return (hashmap_lookup_b(map, key_bytes, key_length) != NULL);
}


/*
Returns 1 if the hashmap contains the given key,
and 0 otherwise.
*/
int hashmap_has_key(hashmap_t* map, const char* key){
    return (hashmap_lookup(map, key) != NULL);
}


/* Retrieves an entry using a byte key. If the entry does not exist, NULL is returned */
void* hashmap_get_b(hashmap_t* map, const void* key, uint32_t key_length) {
    hashmap_entry_t* entry = hashmap_lookup_b(map, key, key_length);
    if (!entry) return NULL;
    return entry->value;
}


/* Retrieves an entry using a key. If the entry does not exist, NULL is returned */
void* hashmap_get(hashmap_t* map, const char* key){
    hashmap_entry_t* entry = hashmap_lookup(map, key);
    if(!entry) return NULL;
    return entry->value;
}


hashmap_t* hashmap_set_b(hashmap_t* map, const void* key, uint32_t key_length, void* value) {
    if (!map || !key || !value) return NULL;

    hashmap_entry_t* entry = hashmap_lookup_b(map, key, key_length);
    uint32_t hash = hashmap_hash_b(key, key_length, map->size);

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
        hashmap_resize_b(map);
    }
    return map;
}


hashmap_t* hashmap_set(hashmap_t* map, const char* key, void* value){
    if(!map || !key || !value) return NULL;

    hashmap_entry_t* entry = hashmap_lookup(map, key);
    uint32_t hash = hashmap_hash(key, map->size);

    while(entry){
        if(streq(key, entry->key)){
            entry->value = value;
            return map;
        }
        entry = entry->next;
    }

    // no matching key found
    entry = calloc(1, sizeof(hashmap_entry_t));
    if(!entry) return NULL;

    entry->key = strdup(key);
    entry->len = strlen(entry->key) + 1; // include null terminator
    entry->value = value;
    entry->next = map->table[hash];
    map->table[hash] = entry;

    // extend if necessary
    map->entries++;
    if(map->entries * HASHMAP_LOADING_FACTOR >= map->size){
        hashmap_resize(map);
    }
    return map;
}


hashmap_t* hashmap_resize_b(hashmap_t* map) {
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
            hashmap_set_b(&new_map, entry->key, entry->len, entry->value);
            entry = entry->next;
        }
    }

    hashmap_uninit(map);
    memmove(map, &new_map, sizeof(hashmap_t));
    return map;
}


hashmap_t* hashmap_resize(hashmap_t* map){
    if(!map) return NULL;

    uint32_t new_size = map->entries * HASHMAP_LOADING_FACTOR;
    hashmap_t new_map;
    hashmap_init(&new_map, new_size);
    hashmap_entry_t* entry;
    uint32_t i;

    // Rehash table
    for(i = 0; i != map->size; ++i){
        entry = map->table[i];
        while(entry){
            hashmap_set(&new_map, entry->key, entry->value);
            entry = entry->next;
        }
    }
    
    hashmap_uninit(map);
    memmove(map, &new_map, sizeof(hashmap_t));
    return map;
}

void* hashmap_iter_keys_b(hashmap_t* map, const char* key, uint32_t key_length, uint32_t* next_key_length) {
    if (!map || !map->table) return NULL;

    hashmap_entry_t* entry = NULL;
    uint32_t hash;

    // search from beginning of table
    if (!key || !(entry = hashmap_lookup_b(map, key, key_length))) {
        for (uint32_t i = 0; i != map->size; ++i) {
            if (map->table[i]) {
                *next_key_length = map->table[i]->len;
                return map->table[i]->key;
            }
        }
        return NULL;
    }

    // search from hash of given key
    if (entry->next) {
        *next_key_length = entry->next->len;
        return entry->next->key;
    }

    hash = hashmap_hash_b(key, key_length, map->size);
    for (uint32_t i = hash + 1; i != map->size; ++i) {
        if (map->table[i]) {
            *next_key_length = map->table[i]->len;
            return map->table[i]->key;
        }
    }
    return NULL;
}


char* hashmap_iter_keys(hashmap_t* map, const char* key){
    if(!map || !map->table) return NULL;
 
    hashmap_entry_t* entry = NULL;
    uint32_t hash;

    // search from beginning of table
    if(!key || !(entry = hashmap_lookup(map,key))){
        for(uint32_t i = 0; i != map->size; ++i){
            if(map->table[i]){
                return map->table[i]->key;
            }
        }
        return NULL;
    }
    
    // search from hash of given key
    if(entry->next){
        return entry->next->key;
    }

    hash = hashmap_hash(key, map->size);
    for(uint32_t i = hash+1; i != map->size; ++i){
            if(map->table[i]){
                return map->table[i]->key;
            }
    }
    return NULL;
}
