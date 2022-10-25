#ifndef LINCE_ARRAY_H
#define LINCE_ARRAY_H

#include <inttypes.h>
#include <stddef.h>

/*
Data structure for dynamic contiguous storage of generic data.
*/
struct array_t {
	void *data;				// main memory pool
	uint32_t size;			// number of stored elements
	uint32_t capacity;		// max number of elements before reallocation
	uint32_t element_size;	// size of an element

	// void on_delete(void* element); // element destructor
};

// -- INITIALIZATIONS
/* Creates a new array of size zero */
struct array_t* array_create(uint32_t element_size);

/*
Initialises an array from existing data
If a size of zero or empty data are provided, no elements are added to the array.
*/
// struct array_t* array_from_data(void* data, uint32_t size, uint32_t element_size);

/*
Pre-allocates a given number of elements but does not initialise them.
If the new size is less than the current one, leftover elements are deleted.
*/
struct array_t* array_resize(struct array_t* array, uint32_t size);

// -- SETTERS
/*
Overwrites an element at the given index with the given data.
If the given data pointer is NULL, the specified element is zeroed.
Returns a pointer to the element in the array.
*/
void* array_set(struct array_t* array, void* data, uint32_t index);

// -- RETRIEVALS
/* Returns a pointer to the element at the specified index */
void* array_get(struct array_t* array, uint32_t index);

/* Returns a pointer to the first element */
void* array_front(struct array_t* array);

/* Returns a pointer to the last element */
void* array_back(struct array_t* array);

/* Returns a pointer to first byte after the end of the array */
void* array_end(struct array_t* array);

// -- INSERTING
/*
Inserts an element at the given index.
If the given element is NULL, the inserted element is zeroed.
*/
struct array_t* array_insert(struct array_t* array, void* element, uint32_t index);

/* Inserts the element to the end of the array */
struct array_t* array_push_back(struct array_t* array, void* element);

/* Inserts the element to the beginning of the array */
struct array_t* array_push_front(struct array_t* array, void* element);


// -- DELETING
/* Removes the element at the given index */
struct array_t* array_delete(struct array_t* array, uint32_t index);

/* Removes the element last element of the array */
struct array_t* array_pop_back(struct array_t* array);

/* Removes the element first element of the array */
struct array_t* array_pop_front(struct array_t* array);

/* Removes all elements on the array */
struct array_t* array_clear(struct array_t* array);

// -- FREEING
/* Frees an array and all of its elements */
void array_destroy(struct array_t* array);

#endif /* LINCE_ARRAY_H */