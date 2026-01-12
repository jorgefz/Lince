/** @file array.h
* `array.h` is an implementation of a resizeable contiguous array containing items of the same type.
* A Dast array stores its data on the heap.
* 
* Example code:
* ```c
* 	array_t data;
* 	array_init(&data, sizeof(float));
*   array_resize(&data, 10);
*
*	for(uint32_t i = 0; i != data.size; ++i){
*       float v = (float)i;
*		array_set(&data, &v, i);
*	}
* 	
* 	float value = 500.0f;
* 	array_push_back(&data, &value);
* 	
*	for(float *item = data.begin; item != data.end; ++item){
*		printf("%.4f ", *item);
*	}
* 	
* 	array_uninit(&data);
* ```
*/


#ifndef DAST_ARRAY_H
#define DAST_ARRAY_H

#include "mem.h"

/** @struct array_t
* @brief Data structure with dynamic contiguous storage of generic data.
*/
typedef struct dast_array {
	void*    data;		    /** < Main memory pool */
	dast_sz size;			/** < Number of stored elements */
	dast_sz capacity;		/** < Number of elements allocated */
	dast_sz element_size;	/** < Size in bytes of an element */
	void* begin; /**< Pointer to the first element of the array.
					  If the array has a size of zero, then begin==end. */
	void* end;   /**< Pointer to the element after the last element of the array.
	                  If the array has a size of zero, then begin==end. */
    dast_allocator_t alloc; /**< Memory allocation functions */
} array_t; /**< Typedef for dast_array */


/** @brief Initialises an array via a given pointer.
*   Should be freed with `array_uninit`.
*	@param array the return array.
*	@param element_size size in bytes of an array element, must be greater than zero
*   @returns `array` if successful, NULL if `array` is NULL or `element_size` is zero.
*   @note Uses standard library malloc-family functions.
*/
array_t* array_init(array_t* array, dast_sz element_size);

/** @brief Initialises an array via a given pointer.
*   Should be freed with `array_uninit`.
*	@param array the return array.
*	@param element_size size in bytes of an array element, must be greater than zero.
*   @param alloc Allocation functions.
*   @returns `array` if successful, NULL if `array` is NULL or `element_size` is zero,
*    or any function in `alloc` is NULL.
*/
array_t* array_init_custom(array_t* array, dast_sz element_size, dast_allocator_t alloc);

/** @brief Resets the array state and frees the memory pool.
*	@param array the array to uninitialise.
*/
void array_uninit(array_t* array);

/** @brief Copies an array into another.
 * The source array must be initialised, and
 * the destination array must *not* be already initialised.
 *	@param dest the resulting copy.
 *  @param src original array to copy.
 *  @returns `dest` if successful, and NULL otherwise.
*/
array_t* array_copy(array_t* dest, array_t* src);

/* Initialises an array from existing data
If a size of zero or empty data are provided, no elements are added to the array.
*/
/* array_t* array_from_data(void* data, uint32_t size, uint32_t element_size); */

/** @brief Pre-allocates a given number of elements.
* The new elements are not initialised.
* Set the value of these elements with `array_set`.
* If the size of an array is reduced, the extra elements are removed.
* @param array Array to resize.
* @param size New size of the array.
*/
array_t* array_resize(array_t* array, dast_sz size);

/** @brief Sets the value of an element.
* Any previously data contained in the element is overwritten.
* If the given pointer to data is NULL, the specified element is filled with zeros.
* Returns a pointer to the element in the array.
* @param array Array for which to modify an element
* @param data Memory which will overwrite the element in question.
* @param index Which element to modify (starting from zero).
*/
void* array_set(array_t* array, void* data, dast_sz index);

/** @brief Returns a pointer to the element at the given index.
* Returns NULL if the index is invalid.
* @param array Array from which to retrieve an element.
* @param index Which element to retrieve.
*/
void* array_get(array_t* array, dast_sz index);

/** @brief Returns a pointer to the first element.
 * If the array has a size of zero, NULL is returned.
*/
void* array_front(array_t* array);

/** @brief Returns a pointer to the last element.
 * If the array has a size of zero, NULL is returned.
*/
void* array_back(array_t* array);

/** @brief Returns a pointer to first byte after the end of the array
 * If the array has a size of zero, then NULL is returned.
*/
void* array_end(array_t* array);

/** @brief Inserts an element at the given index.
* If the given element is NULL, the inserted element is zeroed.
* @param array Array object
* @param element Value to insert
* @param index Location where to insert the element.
* @returns pointer to the inserted item, or NULL
* 	A previous element at this index is displaced one position forward.
*/
void* array_insert(array_t* array, void* element, dast_sz index);

/** @brief Inserts an element at the end of the array
 * @param array array
 * @param element item to insert
 * @returns pointer to the item or NULL
*/
void* array_push_back(array_t* array, void* element);

/** @brief Inserts an element at the beginning of the array
 * @param array array
 * @param element item to insert
 * @returns pointer to the item or NULL
*/
void* array_push_front(array_t* array, void* element);

/** @brief Removes the element at the given index.
 * Note that, whilst the size of the array is reduced, its capacity is not.
 * @param array array
 * @param index index at which to remove an element
 * @returns pointer to the array, or NULL.
*/
array_t* array_remove(array_t* array, dast_sz index);

/** @brief Removes the last element of the array
 * @param array array
 * @returns pointer to the array, or NULL.
*/
array_t* array_pop_back(array_t* array);

/** @brief Removes the element first element of the array
 * @param array array
 * @returns pointer to the array, or NULL.
*/
array_t* array_pop_front(array_t* array);

/** @brief Removes all elements on the array
 * @param array array
 * @returns pointer to the array, or NULL.
*/
array_t* array_clear(array_t* array);

#endif /* DAST_ARRAY_H */