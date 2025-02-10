#ifndef ARRAY_H
#define ARRAY_H

#include <inttypes.h>
#include <stddef.h>

/** @struct array_t
* @brief Data structure with dynamic contiguous storage of generic data.
*/
typedef struct array_container {
	void *data;				///< Main memory pool
	uint32_t size;			///< Number of stored elements
	uint32_t capacity;		///< Number of elements allocated
	uint32_t element_size;	///< Size in bytes of an element
	void* begin; /**< Pointer to the first element of the array.
					  If the array has a size of zero, then begin==end. */
	void* end;   /**< Pointer to the element after the last element of the array.
	                  If the array has a size of zero, then begin==end. */
} array_t;


/** @brief Set custom memory allocation functions.
 * @note Only call this function before any arrays have been initialised
 * @param user_alloc Custom malloc function, allocates block of memory of given size.
 * @param user_realloc Custom realloc function, reallocates existing block of memory into a given size.
 * @param user_free Custom free function, deallocates an allocated block of memory.
 */
void array_set_alloc(
	void* (*user_alloc)  (size_t size),
	void* (*user_realloc)(void* block, size_t size),
	void  (*user_free)   (void* block)
);

/** @brief Initialises an array via a given pointer.
*   Should be freed with `array_uninit`.
*	@param array the return array.
*	@param element_size size in bytes of an array element, must be greater than zero
*   @returns 1 if successful, and 0 otherwise.
*/
int array_init(array_t* array, uint32_t element_size);

/** @brief Resets the array state and frees the memory pool.
*	@param array the array to uninitialise.
*/
void array_uninit(array_t* array);

/** @brief Returns a pointer to a new array allocated on the heap
*   Should be later freed with `array_destroy`.
*	@param element_size size in bytes of an array element.
*/
array_t* array_create(uint32_t element_size);

/** @brief Frees an allocated array
*	@param array the array to deallocate.
*/
void array_destroy(array_t* array); // should also free array_t itself

/** @brief Copies an array into another.
 * The source array must be initialised, and
 * the destination array must *not* be already initialised.
 *	@param dest the resulting copy.
 *  @param src original array to copy.
 *  @returns `dest` if successful, and NULL otherwise.
*/
array_t* array_copy(array_t* dest, array_t* src);

/** @brief Duplicates an array, allocating the new copy on the heap.
*	@param array the array to copy.
*/
array_t* array_new_copy(array_t* orig);

/* Initialises an array from existing data
If a size of zero or empty data are provided, no elements are added to the array.
*/
// array_t* array_from_data(void* data, uint32_t size, uint32_t element_size);

/** @brief Pre-allocates a given number of elements.
* The new elements are not initialised.
* Set the value of these elements with `array_set`.
* If the size of an array is reduced, the extra elements are removed.
* @param array Array to resize.
* @param size New size of the array.
*/
array_t* array_resize(array_t* array, uint32_t size);

/** @brief Sets the value of an element.
* Any previously data contained in the element is overwritten.
* If the given pointer to data is NULL, the specified element is filled with zeros.
* Returns a pointer to the element in the array.
* @param array Array for which to modify an element
* @param data Memory which will overwrite the element in question.
* @param index Which element to modify (starting from zero).
*/
void* array_set(array_t* array, void* data, uint32_t index);

/** @brief Returns a pointer to the element at the given index.
* Returns NULL if the index is invalid.
* @param array Array from which to retrieve an element.
* @param index Which element to retrieve.
*/
void* array_get(array_t* array, uint32_t index);

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
void* array_insert(array_t* array, void* element, uint32_t index);

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
array_t* array_remove(array_t* array, uint32_t index);

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

#endif /* ARRAY_H */