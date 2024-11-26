/** @file memory.h
* @todo rename, potential clash with standard library
*/

#ifndef LINCE_MEMORY_H
#define LINCE_MEMORY_H

#include "lince/core/core.h"

/** @brief `malloc` with checks and assertion of return pointer */
// void* LinceMalloc(size_t nbytes);

/** @brief `calloc` with checks and assertion of return pointer */
// void* LinceCalloc(size_t nbytes);
// #define LinceCalloc(SZ) memset(LinceAlloc(SZ), (char)0, (SZ));

/** @brief `realloc` with checks and assertion of return pointer */
// void* LinceRealloc(void* ptr, size_t nbytes);

/** @brief Frees memory and sets provided pointer to NULL */
// #define LinceFree(p) do{ if(p) LINCE_FREE(p); (p) = NULL; } while(0)

/** @brief Copies `nbytes` from `ptr` into heap-allocated memory */
// void* LinceNewCopy(const void* ptr, size_t nbytes);

#endif /* LINCE_MEMORY_H */