#ifndef LINCE_ALLOCATOR_H
#define LINCE_ALLOCATOR_H

/*

typedef struct LinceAllocStats {
    size_t nblocks; ///< Number of blocks allocated
#ifdef LICNE_DEBUG
    size_t nbytes; ///< Total bytes allocated
#endif
} LinceAllocStats;

typedef struct LinceAllocator {
    void* (*alloc)(size_t size);
    void* (*realloc)(void* block, size_t size);
    void (*free)(void* block);

    LinceAllocStats stats;

#ifdef LINCE_DEBUG
    hashmap_t map;

} LinceAllocator;


void* hashmap_init_alloc(map, size_hint, alloc, realloc, free);
void* array_init_alloc(arr, alloc, realloc, free);



LinceAllocator* LinceInitAllocator(LinceAllocator allocator);
void LinceUninitAllocator();

void LinceGetAllocStats(LinceAllocStats* stats);
*/

#define LinceAlloc(SZ)        LinceMemoryAlloc((SZ), __LINE__, __FILE__, __func__)
#define LinceRealloc(PTR, SZ) LinceMemoryRealloc((PTR), (SZ), __LINE__, __FILE__, __func__)
#define LinceFree(PTR)        LinceMemoryFree((PTR), __LINE__, __FILE__, __func__)

#define LinceCalloc(SZ)       memset(LinceAlloc(SZ), (char)0, (SZ))
#define LinceNewCopy(PTR, SZ) memcpy(LinceAlloc(SZ), (PTR), (SZ))
#define LinceMalloc(SZ)       LinceAlloc(SZ) /* Temporary, until LinceMalloc is replaced with LinceAlloc */

void* LinceMemoryAlloc(size_t size, int line, const char* file, const char* func);
void* LinceMemoryRealloc(void* block, size_t size, int line, const char* file, const char* func);
void LinceMemoryFree(void* block, int line, const char* file, const char* func);


#endif /* LINCE_ALLOCATOR_H */