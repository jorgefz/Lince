#include "containers/array.h"
#include "core/memory.h"

/* Factor by which the capacity is increased */
#define ARRAY_GROW_FACTOR 1.5

#define ARRAY_INITIAL_CAPACITY 1

 // Generic pointer type that allows for pointer arithmetic
typedef char* addr_t;

/*
Returns the nearest highest power of two of an integer
*/
static uint32_t nearest_pow2(uint32_t n){
    if(n <= 1) return 1;
    uint32_t x = 2;
    n--;
	while (n >>= 1) x <<= 1;
	return x;
}

/*
Increments capacity of the array to the next power of two.
*/
static struct array_t* extend_capacity(struct array_t* array){
	if(!array) return NULL;

	uint32_t capacity = array->capacity;
	void* data;

	if(capacity == 0) capacity++;
	else capacity *= 2;

	data = LinceRealloc(array->data, capacity * array->element_size);
	if(!data) return NULL;

	array->data = data;
	array->capacity = capacity;
	return array;
}


// -- INITIALIZATIONS
/* Creates a new array of size zero */
struct array_t* array_create(uint32_t element_size){
	if(element_size == 0) return NULL;
	struct array_t* array;

	array = LinceCalloc(sizeof(struct array_t));
	if(!array) return NULL;

	array->element_size = element_size;
	return array;
}

/* Pre-allocates a given number of elements but does not initialise them */
struct array_t* array_resize(struct array_t* array, uint32_t size){
	if(!array) return NULL;

	if(size <= array->size || size == 0){
		// Shrinking.
		// No need to delete anything. Old data will eventually be overwritten.
		array->size = size;
		return array;
	}
	
	// Round up new capacity to the highest power of two closest to the size
	uint32_t capacity = nearest_pow2(size);

	if(capacity > array->capacity){
		void* data = LinceRealloc(array->data, capacity * array->element_size);
		if(!data) return NULL;
		array->capacity = capacity;
		array->data = data;
	}

	array->size = size;
	return array;
}

// -- SETTERS
/* Overwrites an element at the given index with the given data */
void* array_set(struct array_t* array, void* element, uint32_t index){
	if(!array || index >= array->size) return NULL;
	addr_t addr = array->data + index * array->element_size;
	if(!element){
		memset(addr, 0, array->element_size);
	} else {
		memmove(addr, element, array->element_size);
	}
	return addr;
}

// -- RETRIEVALS
/* Returns a pointer to the element at the specified index */
void* array_get(struct array_t* array, uint32_t index){
	if(!array || index >= array->size) return NULL;
	addr_t addr = array->data + index * array->element_size;
	return addr;
}

/* Returns a pointer to the first element */
void* array_front(struct array_t* array){
	if(!array || array->size == 0) return NULL;
	return array->data;
}

/* Returns a pointer to the last element */
void* array_back(struct array_t* array){
	if(!array || array->size == 0) return NULL;
	return array_get(array, array->size-1);
}

/* Returns a pointer to first byte after the end of the array */
void* array_end(struct array_t* array){
	if(!array || !array->data) return NULL;
	if(array->size == 0){
		return array_back(array);
	} else {
		return (addr_t)array_back(array) + array->element_size;
	}
}

// -- INSERTING
/* Inserts an element at the given index */
struct array_t* array_insert(struct array_t* array, void* element, uint32_t index){
	if(!array || index > array->size){
		return NULL;
	}
	
	addr_t addr = array->data + index;
	uint32_t move_size = array->size - index;

	if(array->size + 1 == array->capacity){
		extend_capacity(array);
	}

	if(move_size > 0){
		// displace elements to make space for new one
		// this operation is invalid if you want to insert at the end of the array
		memmove(addr + array->element_size, addr, move_size * array->element_size);
	}
	if(!element){
		memset(addr, 0, array->element_size);
	} else {
		memmove(addr, element, array->element_size);
	}
	array->size++;
	return array;
}

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
void array_destroy(struct array_t* array){
	if(!array) return;
	if(array->data) LinceFree(array->data);
	LinceFree(array);
}
