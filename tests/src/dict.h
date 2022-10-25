

#ifndef DICT_H
#define DICT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DICT_HASH_SIZE
#define DICT_HASH_SIZE 101
#endif

#ifndef DICT_KEY_MAX
#define DICT_KEY_MAX 100
#endif

struct dict {
	/* data */
	char* key;
	void* value;
	size_t value_size;
	struct dict* next;
};

// Creates an empty dictionary
struct dict** dict_init();

// Destroys a dictionary
void dict_free(struct dict** ht);

/*
Set an element with a given key.
If the key is not present, add new element.
*/
struct dict** dict_set(struct dict** ht, char* key, void* value, size_t value_size);

// Returns the hashed value of a given key
size_t dict_hash(char *key);

/*
Retrieves th keys in a dictionary in order.
Usage:
	char* key = NULL;
	while(1){
		key = dict_get_keys(ht, key);
		if(!key) break;
		printf("%s\n", key);
	}
*/
char* dict_get_keys(struct dict** ht, char* key);


#endif /* DICT_H */


#define DICT_IMPL

// Creates an empty dictionary
struct dict** dict_init(){
	// initialise hash table
	struct dict **ht = calloc(DICT_HASH_SIZE, sizeof(struct dict*));
	return ht;
}

// Destroys a dictionary
void dict_free(struct dict** ht){
	if(ht == NULL) return;
	for(size_t i = 0; i != DICT_HASH_SIZE; ++i){
		if (ht[i] == NULL) continue;
		// free internal element components
		free(ht[i]->value);
		free(ht[i]->key);
		free(ht[i]);
	}
	free(ht);
}

// sets or adds new element
struct dict** dict_set(struct dict** ht, char* key, void* value, size_t value_size){
	struct dict* element = NULL;
	size_t hash = dict_hash(key);
	element = ht[hash];

	// Element is present - update value
	if(element != NULL){
		// provided value has a different type than the stored one
		if(value_size != element->value_size) return NULL;
		memmove(element->value, value, value_size);
		return ht;
	}

	// Add new element
	element = calloc(1, sizeof(struct dict));
	if(element == NULL) return NULL;
	
	element->value_size = value_size;
	element->next = NULL;

	element->key = strndup(key, DICT_KEY_MAX);
	if(element->key == NULL){
		free(element);
		return NULL;
	}

	element->value = malloc(value_size);
	if(element->value == NULL){
		free(element->key);
		free(element);
		return NULL;
	}
	memmove(element->value, value, value_size);

	ht[hash] = element;

	return ht;
}

// finds a given key in a hashtable
static struct dict* dict_lookup(struct dict** ht, char* key){
	struct dict* element;
	for (element = ht[dict_hash(key)]; element != NULL; element = element->next)
        if (strcmp(key, element->key) == 0)
          return element; // found
	return NULL; // not found
}


// Returns the hashed value of a given key
size_t dict_hash(char *key) {
    size_t hashval;
    for (hashval = 0; *key != '\0'; key++)
      hashval = *key + 31 * hashval;
    return hashval % DICT_HASH_SIZE;
}

//Retrieves the keys in a dictionary in order.
char* dict_get_keys(struct dict** ht, char* key){
	if (key == NULL){
		for(size_t i = 0; i != DICT_HASH_SIZE; ++i){
			if(ht[i] == NULL) continue;
			key = ht[i]->key;
			return key;
		}
		return NULL; // empty dict
	}

	size_t hash = dict_hash(key);
	for(size_t i = hash+1; i != DICT_HASH_SIZE; ++i){
		if(ht[i] == NULL) continue;
		key = ht[i]->key;
		return key;
	}
	return NULL;
}



/* Macros */

// Returns the value of an element casted to the provided type
#define dict_get(ht, key, type) *(type*)(ht[dict_hash(key)]->value)

/*
Returns the value of an element casted to the provided type.
If the key is not present, a (given) default value is returned.
*/
#define dict_get_s(ht, key, type, def) (ht[dict_hash(key)]) ? dict_get(ht,key,type) : def


#if 0 /* API design */


int x = 42, y = 12;

struct dict* d = dict_init();

// sets a dictionary member
// if key is present, subsititute value and type if needed
// if key is not present, add new dict element
dict_setp(d, "key", &x, sizeof(int));

// retrieve a pointer to a value
// if key is not present, returns NULL
void* value = dict_getp(d, "key");

// removes an element from the dictionary
// returns NULL if the element is not present
dict_pop(d, "key");

// returns true if dictionary has the given key
dict_has_key(d, "key");

// returns the hashed value of a given key
// in the future, one will be able to specify the hashing algorithm
dict_hash(d, "key")




// fast init
struct dict* d = dict({
	{"x", &x, sizeof(int)},
	{"y", &y, sizeof(int)},
});




/* nice macros */

dict_set(d, "x", int, 15);

int x = dict_get(d, "x", int, 0) // last is default




#endif