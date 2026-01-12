/** @file mem.h
* `mem.h` contains some utility functions memory that substitute existing stdlib functions. 
*/


#ifndef DAST_MEM_H
#define DAST_MEM_H

#include "defs.h"

/* Memory allocation */
typedef void* (*dast_alloc_t)  (dast_sz size);                 /**< Typedef for memory allocation function */ 
typedef void* (*dast_realloc_t)(void* block, dast_sz newsize); /**< Typedef for memory reallocation function */
typedef void  (*dast_free_t)   (void* block);                  /**< Typedef for memory deallocation function */

/** @brief Allocation interface to manage memory blocks on the heap */
typedef struct dast_allocator {
    dast_alloc_t   alloc;   /**< Allocation function   */
    dast_realloc_t realloc; /**< Reallocation function */
    dast_free_t    free;    /**< Deallocation function */
} dast_allocator_t; /**< Typedef for dast_allocator */

/** @brief Returns the default global allocation interface.
 * Unless changed with `dast_set_alloc`, this will the
 * stdlib functions `malloc`, `realloc`, and `free`;
 * or NULL values if usage of the stdlib is disabled (`DAST_NO_STDLIB`).
 */
dast_allocator_t dast_get_alloc(void);

/** @brief Change the default global allocator.
 * This functions are adopted to allocate and free memory on the heap
 * when calling functions that do *not* end in "_custom".
 * @param alloc New default global allocation interface.
 * @note If any function in `alloc` is `dast_null`, the default allocator will
 * fallback to stdlib functions (or `dast_null` if the stdlib is disabled).
 */
void dast_set_alloc(dast_allocator_t alloc);

/** Compares `size` bytes between two memory locations `lhs` and `rhs`.
 * @param lhs First memory location at which to compare data
 * @param rhs Second memory location at which to compare data
 * @param size Number of bytes to compare 
 * @returns `dast_true` if they are equal, and `dast_false` otherwise.
*/
dast_bool dast_memeq(const void* lhs, const void* rhs, dast_sz size);

/** Sets `size` bytes of `dest` to the value of `ch`.
 * @param dest Destination buffer
 * @param ch   Value to set
 * @param size Number of bytes to set
 * @returns `dest` on success, or NULL if `dest` is NULL
 * @note if `ch` is greater than CHAR_MAX, its value is truncated to CHAR_MAX
*/
void* dast_memset(void* dest, int ch, dast_sz size);

/** Copies `size` bytes from `src` to `dest`.
 * @param dest Destination
 * @param src Source location
 * @param size Number of bytes to copy
 * @returns `dest` on success, and NULL if `dest` or `src` are NULL.
 * @note It performs a copy forward, and so it is not suitable for overlapping memory segments.
*/
void* dast_memcpy(void* dest, const void* src, dast_sz size);

/** Copies `size` bytes from `src` to `dest`, handling overlapping segments.
 * @param dest Destination
 * @param src Source location
 * @param size Number of bytes to copy
 * @returns `dest` on success, and NULL if `dest` or `src` are NULL.
 * @note When dest <= src, its behaviour is identical to memcpy.
*/
void* dast_memmove(void* dest, const void* src, dast_sz size);

#endif /* DAST_MEM_H */