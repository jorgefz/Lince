
#include <stdlib.h>

#include "defs.h"
#include "allocator.h"
#include "logger.h"

#include "toml.h"
#include "stb_image_alloc.h"

typedef struct LinceAllocator {
    LinceAllocFn   alloc;   ///< Function to allocate a block of memory of given size
    LinceReallocFn realloc; ///< Function to reallocate a block of memory to a different size
    LinceFreeFn    free;    ///< Function to deallocate a block of memory
    LinceBool initialised;   ///< LinceInitAllocator called
    void* user_data;        ///< Custom user-defined data passed to the allocator functions
    LinceAllocStats stats;  ///< Allocation stats and memory checks
} LinceAllocator;


#ifdef LINCE_DEBUG_MEMCHECK
/** Header data stored in memory before every allocated block */
typedef struct LinceAllocHeader {
    LinceAllocator* allocator; ///< Pointer to _global_allocator. Check for integrity of block memory.
    size_t size; ///< Size of the allocated block.
} LinceAllocHeader;
#endif


/*
 * Wrappers for standard library functions malloc, realloc, and free,
 * which do not take the `user_data` argument that LinceAllocator requires.
 */
static void* LinceStdAllocWrapper(size_t size, void* uptr)                { LINCE_UNUSED(uptr); return malloc(size); }
static void* LinceStdReallocWrapper(void* block, size_t size, void* uptr) { LINCE_UNUSED(uptr); return realloc(block, size); }
static void  LinceStdFreeWrapper(void* block, void* uptr)                 { LINCE_UNUSED(uptr); free(block); }

/* --- Wrappers for external libraries --- */

/* Memory management interface for array functions */
static void* LinceArrayAlloc(size_t size)               { return LinceMemoryAlloc  (size,        0, "array.c", "<array_t function>"); }
static void* LinceArrayRealloc(void* block, size_t size){ return LinceMemoryRealloc(block, size, 0, "array.c", "<array_t function>"); }
static void  LinceArrayFree(void* block)                {        LinceMemoryFree   (block,       0, "array.c", "<array_t function>"); }

/* Memory management interface for hashmap functions */
static void* LinceHashmapAlloc(size_t size)               { return LinceMemoryAlloc  (size,        0, "hashmap.c", "<hashmap_t function>"); }
static void* LinceHashmapRealloc(void* block, size_t size){ return LinceMemoryRealloc(block, size, 0, "hashmap.c", "<hashmap_t function>"); }
static void  LinceHashmapFree(void* block)                {        LinceMemoryFree   (block,       0, "hashmap.c", "<hashmap_t function>"); }

/* Memory management interface for string functions */
static void* LinceStringAlloc(size_t size) { return LinceMemoryAlloc(size,  0, "str.c", "<string_t function>"); }
static void  LinceStringFree(void* block)  {        LinceMemoryFree (block, 0, "str.c", "<string_t function>"); }

/* Memory management interface for stbi_image */
void* LinceSTBIImageAlloc(size_t size)                { return LinceMemoryAlloc  (size,        0, "stb_image.c", "<stb_image function>"); }
void* LinceSTBIImageRealloc(void* block, size_t size) { return LinceMemoryRealloc(block, size, 0, "stb_image.c", "<stb_image function>"); }
void  LinceSTBIImageFree(void* block)                 {        LinceMemoryFree   (block,       0, "stb_image.c", "<stb_image function>"); }

/* Memory management interface for toml */
void* LinceTOMLAlloc(size_t size) { return LinceMemoryAlloc  (size,  0, "toml.c", "<toml function>"); }
void  LinceTOMLFree(void* block)  {        LinceMemoryFree   (block, 0, "toml.c", "<toml function>"); }

/* Global allocators for external libraries */
const dast_allocator_t LINCE_DAST_ARRAY_ALLOCATOR   = {.alloc=LinceArrayAlloc,   .realloc=LinceArrayRealloc,   .free=LinceArrayFree  };
const dast_allocator_t LINCE_DAST_HASHMAP_ALLOCATOR = {.alloc=LinceHashmapAlloc, .realloc=LinceHashmapRealloc, .free=LinceHashmapFree};
const dast_allocator_t LINCE_DAST_STRING_ALLOCATOR  = {.alloc=LinceStringAlloc,  .realloc=NULL,                .free=LinceStringFree };



/** @brief Global allocator for Lince.
 * Defaults to standard library functions malloc, realloc, and free.
 */
static LinceAllocator _global_allocator = {
    .alloc   = LinceStdAllocWrapper,
    .realloc = LinceStdReallocWrapper,
    .free    = LinceStdFreeWrapper,
    .user_data = NULL,
    .stats = {0},
    .initialised = LinceFalse
};

/** @brief Initialise Allocator */
void LinceAllocatorInit(void){
    // stbi_image
    stbi_set_alloc(LinceSTBIImageAlloc, LinceSTBIImageRealloc, LinceSTBIImageFree);
    toml_set_memutil(LinceTOMLAlloc, LinceTOMLFree);
    _global_allocator.initialised = LinceTrue;
}

/** @brief Uninitialise Allocator */
void LinceAllocatorUninit(void){

}

/** @brief Obtain statistics about current memory usage */
void LinceGetAllocStats(LinceAllocStats* stats){
    *stats = _global_allocator.stats;
}

void LinceSetAllocator(LinceAllocFn alloc_fn, LinceReallocFn realloc_fn, LinceFreeFn free_fn, void* user_data){
    if(_global_allocator.initialised){
        LINCE_WARN("Cannot set allocator after it has been initialised");
        return;
    }
    _global_allocator.alloc     = alloc_fn;
    _global_allocator.realloc   = realloc_fn;
    _global_allocator.free      = free_fn;
    _global_allocator.user_data = user_data;
}


void* LinceMemoryAlloc(size_t size, int line, const char* file, const char* func){
    LINCE_UNUSED(line);
    LINCE_UNUSED(file);
    LINCE_UNUSED(func);
    void* block = NULL;

#if defined(LINCE_DEBUG) && defined(LINCE_DEBUG_MEMCHECK)
    LinceAllocHeader* header = _global_allocator.alloc(size + sizeof(LinceAllocHeader), _global_allocator.user_data);
    if(header == NULL){
        LINCE_ERROR("Failed to allocate block of %ld bytes", size + sizeof(LinceAllocHeader));
        LINCE_ERROR("at %s:%d in function '%s'", file, line, func);
        exit(-1);
    }
    header->allocator = &_global_allocator;
    header->size = size;
    block = header + 1;
    _global_allocator.stats.nblocks++;
    _global_allocator.stats.nbytes += (long)size;
    long nblocks = _global_allocator.stats.nblocks;
    _global_allocator.stats.max_blocks = max(nblocks, _global_allocator.stats.max_blocks);
    _global_allocator.stats.max_bytes  = max(_global_allocator.stats.nbytes,  _global_allocator.stats.max_bytes);
    LINCE_INFO("Allocated %*ld byte block at 0x%p (in function %s, %ld total blocks)", 7, size, block, func, nblocks);
    
#elif defined(LINCE_DEBUG) && !defined(LINCE_DEBUG_MEMCHECK)
    _global_allocator.stats.nblocks++;
    _global_allocator.stats.max_blocks = max(_global_allocator.stats.nblocks, _global_allocator.stats.max_blocks);
    block = _global_allocator.alloc(size, _global_allocator.user_data);
#else
    block = _global_allocator.alloc(size, _global_allocator.user_data);
#endif

    return block;
}

void* LinceMemoryRealloc(void* block, size_t size, int line, const char* file, const char* func){
    LINCE_UNUSED(line);
    LINCE_UNUSED(file);
    LINCE_UNUSED(func);
    if(!block){
        return LinceMemoryAlloc(size, line, file, func);
    }

    void* new_block = NULL;

#if defined(LINCE_DEBUG) && defined(LINCE_DEBUG_MEMCHECK)
    LinceAllocHeader* header = (LinceAllocHeader*)block - 1;
    if(header->allocator != &_global_allocator){
        LINCE_ERROR("Attempting to reallocate invalid or corrupted heap pointer 0x%p", block);
        LINCE_ERROR("at %s:%d in function '%s'", file, line, func);
        exit(-1);
    }
    size_t old_size = header->size;
    
    LinceAllocHeader* new_header = _global_allocator.realloc(header, size + sizeof(LinceAllocHeader), _global_allocator.user_data);
    if(new_header == NULL){
        LINCE_ERROR("Failed to allocate block of %ld bytes", size + sizeof(LinceAllocHeader));
        LINCE_ERROR("at %s:%d in function '%s'", file, line, func);
        exit(-1);
    }
    
    new_header->allocator = &_global_allocator;
    new_header->size = size;
    new_block = new_header + 1;
    _global_allocator.stats.nbytes += (long)(size) - (long)(old_size);
    _global_allocator.stats.max_bytes  = max(_global_allocator.stats.nbytes,  _global_allocator.stats.max_bytes);

    LINCE_INFO("Reallocated %*ld byte block to 0x%p, from %ld byte block at 0x%p (in function %s)", 5, size, new_block, old_size, block, func);

#elif defined(LINCE_DEBUG) && !defined(LINCE_DEBUG_MEMCHECK)
    new_block = _global_allocator.realloc(block, size, _global_allocator.user_data);
#else
    new_block = _global_allocator.realloc(block, size, _global_allocator.user_data);
#endif

    return new_block;
}


void LinceMemoryFree(void* block, int line, const char* file, const char* func){

#if defined(LINCE_DEBUG) && defined(LINCE_DEBUG_MEMCHECK)
    if(!block){
        LINCE_WARN("Free called on NULL pointer");
        LINCE_WARN("at %s:%d in function '%s'", file, line, func);
        return;
    }

    LinceAllocHeader* header = (LinceAllocHeader*)block - 1;
    if(header->allocator != &_global_allocator){
        LINCE_WARN("Attempted to free invalid or corrupted heap pointer 0x%p", block);
        LINCE_WARN("at %s:%d in function '%s'", file, line, func);
        return;
    }

    size_t size = header->size;
    _global_allocator.free(header, _global_allocator.user_data);

    _global_allocator.stats.nblocks--;
    _global_allocator.stats.nbytes -= (long)size;

    long nblocks = _global_allocator.stats.nblocks;
    long nbytes = _global_allocator.stats.nbytes;
    LINCE_INFO("Deallocated %*ld byte block at 0x%p (in function %s, %d blocks and %ld bytes in use)", 5, size, block, func, nblocks, nbytes);

#elif defined(LINCE_DEBUG) && !defined(LINCE_DEBUG_MEMCHECK)
    (void)line, (void)file, (void)func;
    _global_allocator.stats.nblocks--;
    _global_allocator.free(block, _global_allocator.user_data);
#else
    (void)line, (void)file, (void)func;
    _global_allocator.free(block, _global_allocator.user_data);
#endif
}

