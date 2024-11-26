
#include <stdlib.h>

#include "defs.h"
#include "allocator.h"
#include "logger.h"

// static LinceAllocator _allocator;

static int nblocks = 0;

void* LinceMemoryAlloc(size_t size, int line, const char* file, const char* func){
    LINCE_UNUSED(line);
    LINCE_UNUSED(file);
    LINCE_UNUSED(func);

    /*
    IN RELEASE MODE
    Make no checks whatsoever.

    IN DEBUG MODE
    Keep track of number of blocks allocated.

    IN MEMCHECK MODE (LINCE_DEBUG_MEMCHECK defined)
    In a hashmap, map a block pointer to info about the allocation,
    including size, line, file, and func.
    Keep track of both total bytes allocated as well as total blocks.
    */

    void* block = malloc(size);
    nblocks++;
    LINCE_INFO("Allocated block of %ld bytes at 0x%p (at %s, %ld total blocks)", size, block, func, nblocks);
    return block;
}

void* LinceMemoryRealloc(void* block, size_t size, int line, const char* file, const char* func){
    LINCE_UNUSED(line);
    LINCE_UNUSED(file);
    LINCE_UNUSED(func);
    
    void* new_block = realloc(block, size);
    return new_block;
}

void LinceMemoryFree(void* block, int line, const char* file, const char* func){
    if(!block){
        LINCE_ERROR("Free called on NULL pointer");
        LINCE_ERROR("at %s:%d in function '%s'", file, line, func);
        exit(-1);
    }

    free(block);
    nblocks--;
    LINCE_INFO("Deallocated block at 0x%p (at %s, %d blocks remain)", block, func, nblocks);
}