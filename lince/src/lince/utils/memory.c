#include "memory.h"
#include "lince/core/logger.h"

/* `malloc` with checks and assertion of return pointer */
/*
void* LinceMalloc(size_t nbytes){
	LINCE_ASSERT(nbytes > 0, "Attempted to allocate zero size memory block");
	void* mem = LINCE_MALLOC(nbytes);
	LINCE_ASSERT_ALLOC(mem, nbytes);
	return mem;
}
*/

/* `calloc` with checks and assertion of return pointer */
/*
void* LinceCalloc(size_t nbytes){
	LINCE_ASSERT(nbytes > 0, "Attempted to allocate zero size memory block");
	void* mem = LINCE_CALLOC(1, nbytes);
	LINCE_ASSERT_ALLOC(mem, nbytes);
	return mem;
}
*/

/* `realloc` with checks and assertion of return pointer */
/*
void* LinceRealloc(void* ptr, size_t nbytes){
	LINCE_ASSERT(nbytes > 0, "Attempted to allocate zero size memory block");
	void *mem = LINCE_REALLOC(ptr, nbytes);
	LINCE_ASSERT_ALLOC(mem, nbytes);
	return mem;
}
*/

/* Copies `nbytes` from `ptr` into heap-allocated memory */
/*
void* LinceNewCopy(const void* ptr, size_t nbytes){
	void* dest = LinceMalloc(nbytes);
	if(!dest) return NULL;
	memmove(dest, ptr, nbytes);
	return dest;
}
*/