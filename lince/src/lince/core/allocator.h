#ifndef LINCE_ALLOCATOR_H
#define LINCE_ALLOCATOR_H

/*

void* hashmap_init_alloc(map, size_hint, alloc, realloc, free);
void* array_init_alloc(arr, alloc, realloc, free);

    IN RELEASE MODE
    Make no checks whatsoever.

    IN DEBUG MODE
    Keep track of number of blocks allocated.

    IN MEMCHECK MODE (LINCE_DEBUG_MEMCHECK defined)
    In a hashmap, map a block pointer to info about the allocation,
    including size, line, file, and func.
    Keep track of both total bytes allocated as well as total blocks.

How to keep track of heap pointers?
Add header to each allocation.

*/

/** @typedef Type signature of function to allocate a block of memory */
typedef void* (*LinceAllocFn)(size_t size, void* user_data);

/** @typedef Type signature of function to reallocate a block of memory */
typedef void* (*LinceReallocFn)(void* block, size_t size, void* user_data);

/** @typedef Type signature of function to deallocate a block of memory */
typedef void  (*LinceFreeFn)(void* block, void* user_data);


typedef struct LinceAllocStats {
    long nblocks; ///< Number of blocks allocated
    long nbytes;  ///< Total bytes allocated
    long max_blocks; ///< Maximum allocated blocks reached
    long max_bytes;  ///< Maximum number of allocated bytes reached
} LinceAllocStats;


// void LinceInitAllocator();
// void LinceUninitAllocator();

#define LinceAlloc(SZ)        LinceMemoryAlloc((SZ), __LINE__, __FILE__, __func__)
#define LinceRealloc(PTR, SZ) LinceMemoryRealloc((PTR), (SZ), __LINE__, __FILE__, __func__)
#define LinceFree(PTR)        LinceMemoryFree((PTR), __LINE__, __FILE__, __func__)

#define LinceCalloc(SZ)       memset(LinceAlloc(SZ), (char)0, (SZ))
#define LinceNewCopy(PTR, SZ) memcpy(LinceAlloc(SZ), (PTR), (SZ))
#define LinceMalloc(SZ)       LinceAlloc(SZ) /* Temporary, until LinceMalloc is replaced with LinceAlloc */


/** @brief Obtain statistics about current memory usage */
void LinceGetAllocStats(LinceAllocStats* stats);

/** @brief Set custom memory management functions */
void LinceSetAllocator(LinceAllocFn alloc_fn, LinceReallocFn realloc_fn, LinceFreeFn free_fn);

/** @brief Memory allocation function that tracks where the memory operation was requested (file, line, and function) */
void* LinceMemoryAlloc(size_t size, int line, const char* file, const char* func);

/** @brief Memory rellocation function that tracks where the memory operation was requested (file, line, and function) */
void* LinceMemoryRealloc(void* block, size_t size, int line, const char* file, const char* func);

/** @brief Memory deallocation function that tracks where the memory operation was requested (file, line, and function) */
void LinceMemoryFree(void* block, int line, const char* file, const char* func);


#endif /* LINCE_ALLOCATOR_H */