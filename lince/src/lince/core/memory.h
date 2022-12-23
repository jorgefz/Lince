#ifndef LINCE_MEMORY_H
#define LINCE_MEMORY_H

#include "lince/core/core.h"

/* `malloc` with checks and assertion of return pointer */
void* LinceMalloc(size_t nbytes);

/* `calloc` with checks and assertion of return pointer */
void* LinceCalloc(size_t nbytes);

/* `realloc` with checks and assertion of return pointer */
void* LinceRealloc(void* ptr, size_t nbytes);

/* Frees memory and sets provided pointer to NULL */
#define LinceFree(p) do{ if(p) LINCE_FREE(p); (p) = NULL; } while(0)

/* Copies `nbytes` from `ptr` into heap-allocated memory */
void* LinceNewCopy(const void* ptr, size_t nbytes);

#endif /* LINCE_MEMORY_H */