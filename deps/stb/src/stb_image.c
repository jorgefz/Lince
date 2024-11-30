/*
 *
 * This .c file serves to compile stb_image.h into as a single translation unit.
 * Do not define `STB_IMAGE_IMPLEMENTATION` elsewhere!!
 *
 */

#include "stdlib.h"
#include "stb_image_alloc.h"

/* Custom allocator for stb_image */
#define STBI_MALLOC(SZ)    stbi_malloc_custom((SZ))
#define STBI_REALLOC(P,SZ) stbi_realloc_custom((P), (SZ))
#define STBI_FREE(P)   stbi_free_custom((P))

void* (*stbi_malloc_custom)(size_t size) = malloc;
void* (*stbi_realloc_custom)(void* block, size_t newsize) = realloc;
void  (*stbi_free_custom)(void* block) = free;

void stbi_set_alloc(
    void* (*umalloc )(size_t       ),
    void* (*urealloc)(void*, size_t),
    void  (*ufree   )(void*        )
){ 
    if(umalloc)  stbi_malloc_custom  = umalloc;
    if(urealloc) stbi_realloc_custom = urealloc;
    if(ufree)    stbi_free_custom    = ufree;
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"