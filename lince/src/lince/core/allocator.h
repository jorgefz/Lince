#ifndef LINCE_ALLOCATOR_H
#define LINCE_ALLOCATOR_H

/*

    IN RELEASE MODE
    Make no checks whatsoever.

    IN DEBUG MODE
    Keep track of number of block and bytes allocated,
    as well as the categories / tags of each allocation.

*/

#include <dast.h>


/** @typedef Type signature of function to allocate a block of memory */
typedef void* (*LinceAllocFn)(size_t size, void* user_data);

/** @typedef Type signature of function to reallocate a block of memory */
typedef void* (*LinceReallocFn)(void* block, size_t size, void* user_data);

/** @typedef Type signature of function to deallocate a block of memory */
typedef void  (*LinceFreeFn)(void* block, void* user_data);

/** @brief Tags which indicate the category or purpose of allocated memory */
typedef enum LinceAllocTag {
    LinceAllocTag_Graphics = 0,
    LinceAllocTag_Assets,
    LinceAllocTag_Other,
    LinceAllocTag_Count
} LinceAllocTag;

/** @brief Stores statistics on total allocated memory */
typedef struct LinceAllocStats {
    long nblocks;    ///< Number of blocks allocated
    long nbytes;     ///< Total bytes allocated
    long max_blocks; ///< Maximum allocated blocks reached
    long max_bytes;  ///< Maximum number of allocated bytes reached
} LinceAllocStats;


/** @brief Helper macro to capture location in source code where an allocation takes place.
 * @param SZ (size_t) Number of bytes to allocate.
 * @returns (void*) Pointer to block of memory of at least `SZ` bytes in size.
 */
#define LinceAlloc(SZ)             LinceMemoryAlloc((SZ), __LINE__, __FILE__, __func__)

/** @brief Helper macro to capture location in source code where allocation takes place
 * as well as its category tag.
 * @param SZ (size_t) Number of bytes to allocate.
 * @param TAG (LinceAllocTag) Category of the memory to allocate.
 * @returns (void*) Pointer to block of memory of at least `SZ` bytes in size.
 */
#define LinceAllocTagged(SZ, TAG)  LinceMemoryAllocTagged((SZ), (TAG), __LINE__, __FILE__, __func__)

/** @brief Helper macro to capture location in source code where a reallocation takes place.
 * @param PTR (void*) Pointer to a previously allocated memory block.
 * @param SZ (size_t) Number of bytes to allocate.
 * @returns (void*) Pointer to block of memory of at least `SZ` bytes in size.
 */
#define LinceRealloc(PTR, SZ)      LinceMemoryRealloc((PTR), (SZ), __LINE__, __FILE__, __func__)

/** @brief Helper macro to capture location in source code where a deallocation takes place.
 * @param PTR (void*) Pointer to a previously allocated memory block.
 */
#define LinceFree(PTR)             LinceMemoryFree((PTR), __LINE__, __FILE__, __func__)

/** @brief Allocate a block of memory in the heap and set its contents to zero.
 * @param SZ (size_t) Number of bytes to allocate.
 * @returns (void*) Pointer to block of memory of at least `SZ` bytes in size.
 */
#define LinceCalloc(SZ)            memset(LinceAlloc(SZ), (char)0, (SZ))

/** @brief Copy `SZ` bytes from `PTR` into a new block of memory allocated on the heap.
 * @param PTR (void*) Pointer to a previously allocated memory block.
 * @param SZ (size_t) Size of the block pointed by `PTR`.
 * @returns (void*) Pointer to new block of size `SZ` storing a copy of as many bytes from `PTR`. 
 */
#define LinceNewCopy(PTR, SZ)      memcpy(LinceAlloc(SZ), (PTR), (SZ))

/** @brief Initialise the engine's allocator.
 */
void LinceAllocatorInit(void);

/** @brief Uninitialise the engine's allocator.
 * @returns The number of allocated memory blocks that have not been freed.
 */
uint64_t LinceAllocatorUninit(void);

/** @brief Obtain statistics about current total memory usage.
 * @param stats LinceAllocStats object to which global statistics are written to.
 */
void LinceAllocatorGetGlobalStats(LinceAllocStats* stats);

/** @brief Obtain statistics about current memory usage per category.
 * @param stats Array of LinceAllocStats of at least `LinceAllocTag_Count` items
 *              to which the per-category statistics will be written to.
 */
void LinceAllocatorGetTaggedStats(LinceAllocStats* stats);

/** @brief Return string name for a memory category */
const char* LinceAllocatorGetTagStringName(LinceAllocTag tag);

/** @brief Set custom memory management functions. Must be called *before* `LinceAllocatorInit()`. */
void LinceAllocatorSet(LinceAllocFn alloc_fn, LinceReallocFn realloc_fn, LinceFreeFn free_fn, void* user_data);

/** @brief Memory allocation function that tracks where the memory operation was requested (file, line, and function)
* as well as the category of the memory (e.g. graphics, assets, etc)
*/
void* LinceMemoryAllocTagged(size_t size, LinceAllocTag tag, int line, const char* file, const char* func);

/** @brief Memory allocation function that tracks where the memory operation was requested (file, line, and function) */
void* LinceMemoryAlloc(size_t size, int line, const char* file, const char* func);

/** @brief Memory rellocation function that tracks where the memory operation was requested (file, line, and function) */
void* LinceMemoryRealloc(void* block, size_t size, int line, const char* file, const char* func);

/** @brief Memory deallocation function that tracks where the memory operation was requested (file, line, and function) */
void LinceMemoryFree(void* block, int line, const char* file, const char* func);




#endif /* LINCE_ALLOCATOR_H */