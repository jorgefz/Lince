
#include <stdlib.h>

#include "defs.h"
#include "allocator.h"
#include "logger.h"


/*
 * Wrappers for standard library functions malloc, realloc, and free,
 * which do not take the `user_data` argument that LinceAllocator requires.
 */
static void* LinceMallocWrapper(size_t size, void* uptr)              { LINCE_UNUSED(uptr); return malloc(size); }
static void* LinceReallocWrapper(void* block, size_t size, void* uptr){ LINCE_UNUSED(uptr); return realloc(block, size); }
static void LinceFreeWrapper(void* block, void* uptr)                 { LINCE_UNUSED(uptr); free(block); }

/** @brief Global allocator for Lince.
 * Defaults to standard library functions malloc, realloc, and free.
 */
static LinceAllocator _global_allocator = {
    .alloc   = LinceMallocWrapper,
    .realloc = LinceReallocWrapper,
    .free    = LinceFreeWrapper,
#ifdef LINCE_DEBUG
    .stats = {0}
#endif
};

static int nblocks = 0;

void LinceSetAllocator(LinceAllocFn alloc, LinceReallocFn realloc, LinceFreeFn free, void* user_data){
    // if(_global_allocator.intialised == LinceTrue){
    //     LINCE_WARN("Cannot set allocator after it has been initialised");
    //     return;
    // }
    _global_allocator.alloc     = alloc;
    _global_allocator.realloc   = realloc;
    _global_allocator.free      = free;
    _global_allocator.user_data = user_data;
}

void* LinceMemoryAlloc(size_t size, int line, const char* file, const char* func){
    LINCE_UNUSED(line);
    LINCE_UNUSED(file);
    LINCE_UNUSED(func);

    void* block = _global_allocator.alloc(size, _global_allocator.user_data);

#ifdef LINCE_DEBUG
    long nblocks = _global_allocator.stats.nblocks++;
    LINCE_INFO("Allocated block of %ld bytes at 0x%p (at %s, %ld total blocks)", size, block, func, nblocks);
#endif
    return block;
}

void* LinceMemoryRealloc(void* block, size_t size, int line, const char* file, const char* func){
    LINCE_UNUSED(line);
    LINCE_UNUSED(file);
    LINCE_UNUSED(func);
    
    void* new_block = _global_allocator.realloc(block, size, _global_allocator.user_data);
    return new_block;
}

void LinceMemoryFree(void* block, int line, const char* file, const char* func){
    if(!block){
        LINCE_ERROR("Free called on NULL pointer");
        LINCE_ERROR("at %s:%d in function '%s'", file, line, func);
        exit(-1);
    }

    _global_allocator.free(block, _global_allocator.user_data);

#ifdef LINCE_DEBUG
    long nblocks = _global_allocator.stats.nblocks--;
    LINCE_INFO("Deallocated block at 0x%p (at %s, %d blocks remain)", block, func, nblocks);
#endif
}