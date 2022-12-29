#ifndef ARRAY_H
#define ARRAY_H

#include <inttypes.h>
#include <stddef.h>

/*
Data structure for dynamic contiguous storage of generic data.
*/
typedef struct array_container {
	void *data;				// main memory pool
	uint32_t size;			// number of stored elements
	uint32_t capacity;		// max number of elements before reallocation
	uint32_t element_size;	// size of an element
} array_t;

// -- CONSTRUCTORS / DESTRUCTORS

/*
Initialises an array via a given pointer.
Should be later freed using `array_uninit`.
*/
void array_init(array_t* array, uint32_t element_size);

/*
Resets the array data without freeing the array object itself.
*/
void array_uninit(array_t* array);

/*
Allocates a new array of size zero.
Should be later freed with `array_destroy`.
*/
array_t* array_create(uint32_t element_size);

/* Frees an allocated array */
void array_destroy(array_t* array); // should also free array_t itself

/*
Initialises an array from existing data
If a size of zero or empty data are provided, no elements are added to the array.
*/
// array_t* array_from_data(void* data, uint32_t size, uint32_t element_size);

/*
Pre-allocates a given number of elements but does not initialise them.
If the new size is less than the current one, leftover elements are removed.
*/
array_t* array_resize(array_t* array, uint32_t size);

// -- SETTERS
/*
Overwrites an element at the given index with the given data.
If the given data pointer is NULL, the specified element is zeroed.
Returns a pointer to the element in the array.
*/
void* array_set(array_t* array, void* data, uint32_t index);

// -- RETRIEVALS
/* Returns a pointer to the element at the specified index */
void* array_get(array_t* array, uint32_t index);

/* Returns a pointer to the first element */
void* array_front(array_t* array);

/* Returns a pointer to the last element */
void* array_back(array_t* array);

/* Returns a pointer to first byte after the end of the array */
void* array_end(array_t* array);

// -- INSERTING
/*
Inserts an element at the given index.
If the given element is NULL, the inserted element is zeroed.
*/
array_t* array_insert(array_t* array, void* element, uint32_t index);

/* Inserts the element to the end of the array */
array_t* array_push_back(array_t* array, void* element);

/* Inserts the element to the beginning of the array */
array_t* array_push_front(array_t* array, void* element);


// -- DELETING
/*
Removes the element at the given index.
Note that, whilst the size of the array is reduced, its capacity cannot be.
*/
array_t* array_remove(array_t* array, uint32_t index);

/* Removes the last element of the array */
array_t* array_pop_back(array_t* array);

/* Removes the element first element of the array */
array_t* array_pop_front(array_t* array);

/* Removes all elements on the array */
array_t* array_clear(array_t* array);

#endif /* ARRAY_H */