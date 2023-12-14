
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

// Evaluates to true (1) if both strings are equal
static int streq(const char* s1, const char* s2){
    return (s1 && s2) && ((s1 == s2) || (strcmp(s1,s2) == 0));
}

// Returns the hashmap element with the given key
static hm_entry_t* hashmap_lookup(hashmap_t* map, const char* key){
    if(!map || !key) return NULL;
    uint32_t hash = hashmap_hash(key, map->size);
    hm_entry_t* entry = map->table[hash];

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

uint32_t hashmap_hash_bytes(const char* key, uint32_t length, uint32_t mapsize) {
    // Using 'one-at-a-time' hashing function by Bob Jenkins
    // https://en.wikipedia.org/wiki/Jenkins_hash_function
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash % mapsize;
}

uint32_t hashmap_hash(const char* key, uint32_t mapsize) {
    // Using 'one-at-a-time' hashing function by Bob Jenkins
    // https://en.wikipedia.org/wiki/Jenkins_hash_function
    return hashmap_hash_bytes(key, strlen(key), mapsize);
}

/*
Initialises hashmap via passed pointer.
Should be deleted using `hashmap_uninit`.
*/
int hashmap_init(hashmap_t* map, uint32_t size_hint){
    *map = (hashmap_t){0};
    map->size = next_prime(size_hint);
    map->table = calloc(map->size, sizeof(hm_entry_t*));
    if(!map->table) return 1;
    return 0;
}

/* Clears and deallocates a hashmap */
void hashmap_uninit(hashmap_t* map){
    if(!map || !map->table) return;

    for(uint32_t i=0; i!=map->size; ++i){
        hm_entry_t* entry = map->table[i], *next;
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
Returns 1 if the hashmap contains the given key,
and 0 otherwise.
*/
int hashmap_has_key(hashmap_t* map, const char* key){
    return (hashmap_lookup(map, key) != NULL);
}


/* Retrieves an entry using a key. If the entry does not exist, NULL is returned */
void* hashmap_get(hashmap_t* map, const char* key){
    hm_entry_t* entry = hashmap_lookup(map, key);
    if(!entry) return NULL;
    return entry->value;
}


hashmap_t* hashmap_set(hashmap_t* map, const char* key, void* value){
    if(!map || !key || !value) return NULL;

    hm_entry_t* entry = hashmap_lookup(map, key);
    uint32_t hash = hashmap_hash(key, map->size);

    while(entry){
        if(streq(key, entry->key)){
            entry->value = value;
            return map;
        }
        entry = entry->next;
    }

    // no matching key found
    entry = calloc(1, sizeof(hm_entry_t));
    if(!entry) return NULL;

    entry->key = strdup(key);
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


hashmap_t* hashmap_resize(hashmap_t* map){
    if(!map) return NULL;

    uint32_t new_size = map->entries * HASHMAP_LOADING_FACTOR;
    hashmap_t new_map;
    hashmap_init(&new_map, new_size);
    hm_entry_t* entry;
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


char* hashmap_iter_keys(hashmap_t* map, const char* key){
    if(!map || !map->table) return NULL;
 
    hm_entry_t* entry = NULL;
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
