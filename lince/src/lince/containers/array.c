#include <stdlib.h>
#include <string.h>
#include "containers/array.h"

/* Factor by which the capacity is increased */
#define ARRAY_GROW_FACTOR 1.5

#define ARRAY_INITIAL_CAPACITY 1


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
static array_t* extend_capacity(array_t* array){
	if(!array) return NULL;

	uint32_t capacity = array->capacity;
	void* data;

	if(capacity == 0) capacity++;
	else capacity *= 2;

	data = realloc(array->data, capacity * array->element_size);
	if(!data) return NULL;

	array->data = data;
	array->capacity = capacity;
	return array;
}


// -- INITIALIZATIONS

/*
Initialises an array via a given pointer.
Should be later freed using `array_uninit`.
*/
int array_init(array_t* array, uint32_t element_size){
	if(!array) return 0;
	*array = (array_t){0};
	if(element_size == 0) return 0;
	array->element_size = element_size;
	return 1;
}

/*
Deallocates and resets the array data without freeing the array object itself.
*/
void array_uninit(array_t* array){
	if(!array) return;
	if(array->data) free(array->data);
	*array = (array_t){0};
}


/* Creates a new array of size zero */
array_t* array_create(uint32_t element_size){
	if(element_size == 0) return NULL;
	array_t* array = calloc(1, sizeof(array_t));
	if(!array) return NULL;
	array->element_size = element_size;
	return array;
}

/* Frees all the elements of an array */
void array_destroy(array_t* array){
	if(!array) return;
	if(array->data) free(array->data);
	free(array);
}

array_t* array_copy(array_t* orig){
	if(!orig) return NULL;
	array_t* new = malloc(sizeof(array_t));
	if(!new) return NULL;

	memmove(new, orig, sizeof(array_t));
	if(!orig->data){
		return new;
	}

	size_t bytes = orig->capacity * orig->element_size;
	new->data = malloc(bytes);
	if(!new->data) return NULL;
	memmove(new->data, orig->data, bytes);
	new->begin = new->data;
	new->end = array_end(new);

	return new;
}

/* Pre-allocates a given number of elements but does not initialise them */
array_t* array_resize(array_t* array, uint32_t size){
	if(!array || array->element_size == 0) return NULL;

	if(size <= array->size){
		// Shrinking
		// No need to delete anything. Old data will eventually be overwritten.
		array->size = size;
		array->end = array_end(array);
		return array;
	}
	
	// Round up new capacity to the highest power of two closest to the size
	uint32_t capacity = nearest_pow2(size);
	if(capacity > array->capacity){
		void* data = realloc(array->data, capacity * array->element_size);
		if(!data) return NULL;
		array->capacity = capacity;
		array->data = data;
	}

	array->size = size;
	array->begin = array->data;
	array->end = array_end(array);
	return array;
}

// -- SETTERS
/* Overwrites an element at the given index with the given data */
void* array_set(array_t* array, void* element, uint32_t index){
	if(!array || array->element_size == 0 || index >= array->size) return NULL;
	char* addr = (char*)array->data + index * array->element_size;
	if(!element){
		memset(addr, 0, array->element_size);
	} else {
		memmove(addr, element, array->element_size);
	}
	return addr;
}

// -- RETRIEVALS
/* Returns a pointer to the element at the specified index */
void* array_get(array_t* array, uint32_t index){
	if(!array || array->element_size == 0 || index >= array->size) return NULL;
	char* addr = (char*)array->data + index * array->element_size;
	return addr;
}

/* Returns a pointer to the first element */
void* array_front(array_t* array){
	if(!array || array->size == 0) return NULL;
	return array->data;
}

/* Returns a pointer to the last element */
void* array_back(array_t* array){
	if(!array || array->size == 0) return NULL;
	return array_get(array, array->size-1);
}

/* Returns a pointer to first byte after the end of the array */
void* array_end(array_t* array){
	if(!array || !array->data || array->element_size == 0) return NULL;
	if(array->size == 0){
		return array_back(array);
	}
	return (char*)array_back(array) + array->element_size;
}

// -- INSERTING
/* Inserts an element at the given index */
array_t* array_insert(array_t* array, void* element, uint32_t index){
	if(!array || array->element_size == 0 || index > array->size){
		return NULL;
	}

	if(array->size >= array->capacity || !array->data){
		array_t* r = extend_capacity(array);
		if(!r) return NULL;
	}
	
	char* addr = (char*)array->data + index * array->element_size;
	uint32_t move_bytes = (array->size - index) * array->element_size;
	
	if(move_bytes > 0){
		// displace elements to make space for new one
		// this operation is invalid if you want to insert at the end of the array
		memmove(addr + array->element_size, addr, move_bytes);
	}

	// setting value
	if(!element){
		memset(addr, 0, array->element_size);
	} else {
		memmove(addr, element, array->element_size);
	}
	array->size++;
	array->begin = array->data;
	array->end = array_end(array);
	return array;
}

/* Inserts the element to the end of the array */
array_t* array_push_back(array_t* array, void* element){
	return array_insert(array, element, array->size);
}

/* Inserts the element to the beginning of the array */
array_t* array_push_front(array_t* array, void* element){
	return array_insert(array, element, 0);
}

// -- DELETING
/* Removes the element at the given index */
array_t* array_remove(array_t* array, uint32_t index){
	if(!array || !array->data || index >= array->size){
		return NULL;
	}

	if (index == array->size - 1){
		// pop back, no need to shuffle data around
		array->size--;
		return array;
	}
	
	char* dest = (char*)array->data + index * array->element_size;
	char* orig = dest + array->element_size;
	uint32_t move_bytes = (array->size - index) * array->element_size;

	memmove(dest, orig, move_bytes);
	array->size--;
	array->begin = array->data;
	array->end = array_end(array);

	return array;
}

/* Removes the element last element of the array */
array_t* array_pop_back(array_t* array){
	if(array->size == 0) return NULL;
	return array_remove(array, array->size-1);
}

/* Removes the element first element of the array */
array_t* array_pop_front(array_t* array){
	return array_remove(array, 0);
}

/* Removes all elements on the array */
array_t* array_clear(array_t* array){
	if(!array) return NULL;
	array->size = 0;
	array->begin = array->data;
	array->end = array->begin;
	return array;
}

