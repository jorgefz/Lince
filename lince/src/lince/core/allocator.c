
#include <stdlib.h>

#include "defs.h"
#include "allocator.h"
#include "logger.h"

#include "toml.h"
#include "stb_image_alloc.h"
#include "dast.h"

typedef struct LinceAllocator {
    // Principal interface
    LinceAllocFn   alloc;   ///< Function to allocate a block of memory of given size
    LinceReallocFn realloc; ///< Function to reallocate a block of memory to a different size
    LinceFreeFn    free;    ///< Function to deallocate a block of memory

    // Internals
    LinceBool initialised;   ///< LinceInitAllocator called
    LinceAllocConfig config; ///< Configuration options
    void* user_data;         ///< Custom user-defined data passed to the allocator functions
    LinceAllocStats stats;   ///< Allocation stats and memory checks
    LinceAllocStats tag_stats[LinceAllocTag_Count]; ///< Allocation stats per category
} LinceAllocator;


/** @brief Header data stored in memory before every allocated block
* which provides additional context about the allocated memory block.
* @note The "_pad" member exists only to ensure the struct is 32 bytes in size. 
*/
typedef struct LinceAllocHeader {
    LinceAllocator* allocator; ///< Pointer to _global_allocator. Check for integrity of block memory.
    size_t  size;    ///< Size of the allocated block.
    int32_t tag;     ///< Category tag
    int32_t _pad[3]; ///< Padding to ensure sizeof(LinceAllocHeader) == 32 bytes.
} LinceAllocHeader;


/*
 * Wrappers for standard library functions malloc, realloc, and free,
 * which do not take the `user_data` argument that LinceAllocator requires.
 */
static void* LinceStdAllocWrapper(size_t size, void* uptr)                { LINCE_UNUSED(uptr); return malloc(size); }
static void* LinceStdReallocWrapper(void* block, size_t size, void* uptr) { LINCE_UNUSED(uptr); return realloc(block, size); }
static void  LinceStdFreeWrapper(void* block, void* uptr)                 { LINCE_UNUSED(uptr); free(block); }

/* --- Wrappers for external libraries --- */

/* Memory management interface for DAST functions (string, array, hashmap) */
static void* LinceDASTAlloc(size_t size)               { return LinceMemoryAlloc  (size,        0, "dast.h", "<dast function>"); }
static void* LinceDASTRealloc(void* block, size_t size){ return LinceMemoryRealloc(block, size, 0, "dast.h", "<dast function>"); }
static void  LinceDASTFree(void* block)                {        LinceMemoryFree   (block,       0, "dast.h", "<dast function>"); }

/* Memory management interface for stbi_image */
void* LinceSTBIImageAlloc(size_t size)                { return LinceMemoryAllocTagged(size,    LinceAllocTag_Assets, 0, "stb_image.c", "<stb_image function>"); }
void* LinceSTBIImageRealloc(void* block, size_t size) { return LinceMemoryRealloc(block, size, 0, "stb_image.c", "<stb_image function>"); }
void  LinceSTBIImageFree(void* block)                 {        LinceMemoryFree   (block,       0, "stb_image.c", "<stb_image function>"); }

/* Memory management interface for toml */
void* LinceTOMLAlloc(size_t size) { return LinceMemoryAllocTagged(size, LinceAllocTag_Assets, 0, "toml.c", "<toml function>"); }
void  LinceTOMLFree(void* block)  {        LinceMemoryFree   (block, 0, "toml.c", "<toml function>"); }


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
void LinceAllocatorInit(LinceAllocConfig* config){
    
    if(config){
        _global_allocator.config = *config;
    } else {
        LinceAllocatorGetDefaultConfig(&_global_allocator.config);
    }

    // Set up external allocators
    stbi_set_alloc(LinceSTBIImageAlloc, LinceSTBIImageRealloc, LinceSTBIImageFree);
    toml_set_memutil(LinceTOMLAlloc, LinceTOMLFree);
    dast_set_alloc((dast_allocator_t){LinceDASTAlloc, LinceDASTRealloc, LinceDASTFree});
    
    _global_allocator.initialised = LinceTrue;
}

/** @brief Uninitialise Allocator */
uint64_t LinceAllocatorUninit(void){

    LINCE_INFO("Allocator: peak of %lu blocks allocated (%.2f kb)",
        _global_allocator.stats.max_blocks,
        (double)_global_allocator.stats.max_bytes / 1024.0);

    uint64_t unfreed_blocks = _global_allocator.stats.nblocks;
    if(unfreed_blocks != 0){
        LINCE_WARN("Allocator warning: %u unfreed blocks (%u bytes)",
            unfreed_blocks, _global_allocator.stats.nbytes);
    }
    _global_allocator.initialised = LinceFalse;
    _global_allocator.stats = (LinceAllocStats){0};
    LINCE_INFO("Allocator uninitialised");

    return unfreed_blocks;
}

/** @brief Return the default configuration of the allocator.
 * @param config Output location to which to write default allocator configuration.
 */
void LinceAllocatorGetDefaultConfig(LinceAllocConfig* config){
    config->memcheck = LinceFalse;
}

/** @brief Obtain statistics about current memory usage */
void LinceAllocatorGetGlobalStats(LinceAllocStats* stats){
    *stats = _global_allocator.stats;
}

/** @brief Obtain statistics about current memory usage per category */
void LinceAllocatorGetTaggedStats(LinceAllocStats *stats){
    for(LinceAllocTag tag = 0; tag != LinceAllocTag_Count; tag++){
        stats[tag] = _global_allocator.tag_stats[tag];
    }
}

/** @brief Return string name for memory category */
const char* LinceAllocatorGetTagStringName(LinceAllocTag tag){
    switch(tag){
        case LinceAllocTag_Graphics:
            return "Graphics";
        case LinceAllocTag_Assets:
            return "Assets";
        case LinceAllocTag_Other:
            return "Other";
        default:
            return "Unknown";
    }
    return NULL;
}


void LinceAllocatorSet(LinceAllocFn alloc_fn, LinceReallocFn realloc_fn, LinceFreeFn free_fn, void* user_data){
    if(_global_allocator.initialised){
        LINCE_WARN("Cannot set allocator after it has been initialised");
        return;
    }
    _global_allocator.alloc     = alloc_fn;
    _global_allocator.realloc   = realloc_fn;
    _global_allocator.free      = free_fn;
    _global_allocator.user_data = user_data;
}


void* LinceMemoryAllocTagged(size_t size, LinceAllocTag tag, int line, const char* file, const char* func){
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
    header->tag  = tag;
    block = header + 1;

    _global_allocator.stats.nblocks++;
    _global_allocator.stats.nbytes += (long)size;
    long nblocks = _global_allocator.stats.nblocks;
    _global_allocator.stats.max_blocks = LINCE_MAX(nblocks, _global_allocator.stats.max_blocks);
    _global_allocator.stats.max_bytes  = LINCE_MAX(_global_allocator.stats.nbytes,  _global_allocator.stats.max_bytes);
    // LINCE_INFO("Allocated %*ld byte block at 0x%p for %s (in function %s, %ld total blocks)",
    //     7, size, block, LinceAllocatorGetTagStringName(tag), func, nblocks);

    if(tag < LinceAllocTag_Count){
        _global_allocator.tag_stats[tag].nblocks++;
        _global_allocator.tag_stats[tag].nbytes += (long)size;
        long tag_nblocks = _global_allocator.tag_stats[tag].nblocks;
        _global_allocator.tag_stats[tag].max_blocks = LINCE_MAX(tag_nblocks, _global_allocator.tag_stats[tag].max_blocks);
        _global_allocator.tag_stats[tag].max_bytes  = LINCE_MAX(_global_allocator.tag_stats[tag].nbytes,  _global_allocator.tag_stats[tag].max_bytes);
    }

    
#elif defined(LINCE_DEBUG) && !defined(LINCE_DEBUG_MEMCHECK)
    LINCE_UNUSED(tag);
    _global_allocator.stats.nblocks++;
    _global_allocator.stats.max_blocks = LINCE_MAX(_global_allocator.stats.nblocks, _global_allocator.stats.max_blocks);

    block = _global_allocator.alloc(size, _global_allocator.user_data);
#else
    block = _global_allocator.alloc(size, _global_allocator.user_data);
#endif

    return block;
}


void* LinceMemoryAlloc(size_t size, int line, const char* file, const char* func){
    return LinceMemoryAllocTagged(size, LinceAllocTag_Other, line, file, func);
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
        LINCE_ERROR("Attempting to reallocate invalid or corrupted heap pointer 0x%p for %s", block, LinceAllocatorGetTagStringName(header->tag));
        LINCE_ERROR("at %s:%d in function '%s'", file, line, func);
        exit(-1);
    }
    size_t old_size = header->size;
    LinceAllocTag tag = header->tag;
    
    LinceAllocHeader* new_header = _global_allocator.realloc(header, size + sizeof(LinceAllocHeader), _global_allocator.user_data);
    if(new_header == NULL){
        LINCE_ERROR("Failed to allocate block of %ld bytes for %s", size + sizeof(LinceAllocHeader), LinceAllocatorGetTagStringName(header->tag));
        LINCE_ERROR("at %s:%d in function '%s'", file, line, func);
        exit(-1);
    }
    
    new_header->allocator = &_global_allocator;
    new_header->size = size;
    new_header->tag  = tag;
    new_block = new_header + 1;
    _global_allocator.stats.nbytes += (long)(size) - (long)(old_size);
    _global_allocator.stats.max_bytes  = LINCE_MAX(_global_allocator.stats.nbytes,  _global_allocator.stats.max_bytes);
    // LINCE_INFO("Reallocated %*ld byte block to 0x%p for %s, from %ld byte block at 0x%p (in function %s)",
    //     5, size, new_block, LinceAllocatorGetTagStringName(header->tag), old_size, block, func);
    
    if(tag < LinceAllocTag_Count){
        _global_allocator.tag_stats[tag].nbytes += (long)(size) - (long)(old_size);
        _global_allocator.tag_stats[tag].max_bytes  = LINCE_MAX(_global_allocator.tag_stats[tag].nbytes,  _global_allocator.tag_stats[tag].max_bytes);
    }


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
        LINCE_WARN("Free called on NULL pointer at %s:%d in function '%s'", file, line, func);
        return;
    }

    LinceAllocHeader* header = (LinceAllocHeader*)block - 1;
    if(header->allocator != &_global_allocator){
        LINCE_WARN("Attempted to free invalid or corrupted heap pointer 0x%p at %s:%d in function '%s'", block, file, line, func);
        return;
    }

    size_t size = header->size;
    LinceAllocTag tag = header->tag;
    _global_allocator.free(header, _global_allocator.user_data);

    _global_allocator.stats.nblocks--;
    _global_allocator.stats.nbytes -= (long)size;

    long nblocks = _global_allocator.stats.nblocks;
    long nbytes = _global_allocator.stats.nbytes;
    // LINCE_INFO("Deallocated %*ld byte block at 0x%p for %s (in function %s, %d blocks and %ld bytes in use)",
    //     5, size, block, LinceAllocatorGetTagStringName(tag), func, nblocks, nbytes);

    if(tag < LinceAllocTag_Count){
        _global_allocator.tag_stats[tag].nblocks--;
        _global_allocator.tag_stats[tag].nbytes -= (long)size;
    }

#elif defined(LINCE_DEBUG) && !defined(LINCE_DEBUG_MEMCHECK)
    (void)line, (void)file, (void)func;
    _global_allocator.stats.nblocks--;
    _global_allocator.free(block, _global_allocator.user_data);
#else
    (void)line, (void)file, (void)func;
    _global_allocator.free(block, _global_allocator.user_data);
#endif
}

