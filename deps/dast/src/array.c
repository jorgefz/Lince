#include "array.h"


/* Returns the nearest highest power of two of an integer */
static dast_sz dast_nearest_pow2(dast_sz n){
    dast_sz x = 2;
    if(n <= 1) return 1;
    n--;
	while (n >>= 1) x <<= 1;
	return x;
}


/* Increments capacity of the array to the next power of two */
static array_t* array_extend_capacity(array_t* array){
	dast_sz capacity;
    void* data;

    if(!array) return dast_null;
	capacity = array->capacity;

	if(capacity == 0) capacity++;
	else capacity *= 2;

	data = array->alloc.realloc(array->data, capacity * array->element_size);
	if(!data) return dast_null;

	array->data = data;
	array->capacity = capacity;
	return array;
}


/* -- INITIALIZATIONS -- */

/*
Initialises an array via a given pointer.
Should be later freed using `array_uninit`.
*/
array_t* array_init(array_t* array, dast_sz element_size){
	return array_init_custom(array, element_size, dast_get_alloc());
}

#include <stdio.h>

/** @brief Initialises an array via a given pointer with custom memory allocation functions.
*   Should be freed with `array_uninit`.
*/
array_t* array_init_custom(array_t* array, dast_sz element_size, dast_allocator_t alloc){
	if(!array || !alloc.alloc || !alloc.realloc || !alloc.free) return dast_null;
    if(element_size == 0) return dast_null;
    *array = (array_t){0};
    array->element_size = element_size;
    array->alloc = alloc;
    return array;
}

/*
Deallocates and resets the array data without freeing the array object itself.
*/
void array_uninit(array_t* array){
	if(!array) return;
	if(array->data) array->alloc.free(array->data);
	*array = (array_t){0};
}

/** @brief Copies an array into another.
 * The source array must be initialised, and
 * the destination array must *not* be already initialised.
 *	@param dest the resulting copy.
 *  @param src original array to copy.
 *  @returns `dest` if successful, and NULL otherwise.
*/
array_t* array_copy(array_t* dest, array_t* src){
	if(!dest || !src) return dast_null;
	
	void* success = array_init_custom(dest, src->element_size, src->alloc);
	if(!success) return dast_null;
	if(src->size == 0) return dest;

	if(!array_resize(dest, src->size)){
		array_uninit(dest);
		return dast_null;
	}

	dast_memcpy(dest->data, src->data, dest->size * dest->element_size);
	return dest;
}


/* Pre-allocates a given number of elements but does not initialise them */
array_t* array_resize(array_t* array, dast_sz size){
	dast_sz capacity;
    void* data;

    if(!array || array->element_size == 0){
        return dast_null;
    }

	if(size <= array->size){
		/* Shrinking - No need to delete anything. Old data will eventually be overwritten. */
		array->size  = size;
		array->end   = array_end(array);
		array->begin = array_front(array);
		return array;
	}
	
	/* Round up new capacity to the highest power of two closest to the size */
	capacity = dast_nearest_pow2(size);
	if(capacity > array->capacity){
		data = array->alloc.realloc(array->data, capacity * array->element_size);
		if(!data) return dast_null;
		array->capacity = capacity;
		array->data = data;
	}

	array->size = size;
	array->begin = array->data;
	array->end = array_end(array);
	return array;
}

/* -- SETTERS -- */
/* Overwrites an element at the given index with the given data */
void* array_set(array_t* array, void* element, dast_sz index){
	char* addr;
    if(!array || array->element_size == 0 || index >= array->size) return dast_null;
    addr = (char*)array->data + index * array->element_size;
	if(!element){
		dast_memset(addr, 0, array->element_size);
	} else {
		dast_memmove(addr, element, array->element_size);
	}
	return addr;
}

/* -- RETRIEVALS -- */
/* Returns a pointer to the element at the specified index */
void* array_get(array_t* array, dast_sz index){
    char* addr;
    if(!array || array->element_size == 0 || index >= array->size) return dast_null;
	addr = (char*)array->data + index * array->element_size;
	return addr;
}

/* Returns a pointer to the first element */
void* array_front(array_t* array){
	if(!array || array->size == 0) return dast_null;
	return array->data;
}

/* Returns a pointer to the last element */
void* array_back(array_t* array){
	if(!array || array->size == 0) return dast_null;
	return array_get(array, array->size-1);
}

/* Returns a pointer to first byte after the end of the array */
void* array_end(array_t* array){
	if(!array || !array->data || array->element_size == 0 || array->size == 0){
		return dast_null;
	}
	return (char*)array_back(array) + array->element_size;
}

/* -- INSERTING -- */
/* Inserts an element at the given index */
void* array_insert(array_t* array, void* element, dast_sz index){
	array_t* r;
	char* addr;
    dast_sz move_bytes;

    if(!array || array->element_size == 0 || index > array->size){
		return dast_null;
	}

	if(array->size >= array->capacity || !array->data){
		r = array_extend_capacity(array);
		if(!r) return dast_null;
	}
	
	addr = (char*)(array->data) + index * array->element_size;
	move_bytes = (array->size - index) * array->element_size;
	
	if(move_bytes > 0){
		/* Displace elements to make space for new one.
		   This operation is invalid if you want to insert at the end of the array. */
		dast_memmove(addr + array->element_size, addr, move_bytes);
	}

	if(!element){
		dast_memset(addr, 0, array->element_size);
	} else {
		dast_memmove(addr, element, array->element_size);
	}
	array->size++;
	array->begin = array->data;
	array->end = array_end(array);
	return addr;
}

/* Inserts the element to the end of the array */
void* array_push_back(array_t* array, void* element){
	return array_insert(array, element, array->size);
}

/* Inserts the element to the beginning of the array */
void* array_push_front(array_t* array, void* element){
	return array_insert(array, element, 0);
}

/* -- DELETING -- */
/* Removes the element at the given index */
array_t* array_remove(array_t* array, dast_sz index){
    dast_sz move_bytes;
    char *dest, *orig;

	if(!array || !array->data || index >= array->size){
		return dast_null;
	}

	if (index == array->size - 1){
		/* Pop back, no need to shuffle data around */
		array->size--;
		return array;
	}
	
	dest = (char*)array->data + index * array->element_size;
	orig = dest + array->element_size;
	move_bytes = (array->size - index) * array->element_size;

	dast_memmove(dest, orig, move_bytes);
	array->size--;
	array->begin = array->data;
	array->end = array_end(array);

	return array;
}

/* Removes the element last element of the array */
array_t* array_pop_back(array_t* array){
	if(array->size == 0) return dast_null;
	return array_remove(array, array->size-1);
}

/* Removes the element first element of the array */
array_t* array_pop_front(array_t* array){
	return array_remove(array, 0);
}

/* Removes all elements on the array */
array_t* array_clear(array_t* array){
	if(!array) return dast_null;
	array->size = 0;
	array->begin = dast_null;
	array->end   = dast_null;
	return array;
}

