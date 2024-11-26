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

*/


/** @typedef Type signature of function to allocate a block of memory */
typedef void* (*LinceAllocFn)(size_t size, void* user_data);

/** @typedef Type signature of function to reallocate a block of memory */
typedef void* (*LinceReallocFn)(void* block, size_t size, void* user_data);

/** @typedef Type signature of function to deallocate a block of memory */
typedef void* (*LinceFreeFn)(void* block, void* user_data);


#ifdef LINCE_DEBUG
typedef struct LinceAllocStats {
    long nblocks;        ///< Number of blocks allocated
    #ifdef LINCE_DEBUG_MEMCHECK
    long nbytes; ///< Total bytes allocated
    #endif
} LinceAllocStats;
#endif


// void LinceInitAllocator();
// void LinceUninitAllocator();

#define LinceAlloc(SZ)        LinceMemoryAlloc((SZ), __LINE__, __FILE__, __func__)
#define LinceRealloc(PTR, SZ) LinceMemoryRealloc((PTR), (SZ), __LINE__, __FILE__, __func__)
#define LinceFree(PTR)        LinceMemoryFree((PTR), __LINE__, __FILE__, __func__)

#define LinceCalloc(SZ)       memset(LinceAlloc(SZ), (char)0, (SZ))
#define LinceNewCopy(PTR, SZ) memcpy(LinceAlloc(SZ), (PTR), (SZ))
#define LinceMalloc(SZ)       LinceAlloc(SZ) /* Temporary, until LinceMalloc is replaced with LinceAlloc */


#ifdef LINCE_DEBUG
void LinceGetAllocStats(LinceAllocStats* stats);
#endif

void LinceSetAllocator(LinceAllocFn alloc, LinceReallocFn realloc, LinceFreeFn free, void* user_data);

void* LinceMemoryAlloc(size_t size, int line, const char* file, const char* func);
void* LinceMemoryRealloc(void* block, size_t size, int line, const char* file, const char* func);
void LinceMemoryFree(void* block, int line, const char* file, const char* func);


#endif /* LINCE_ALLOCATOR_H */