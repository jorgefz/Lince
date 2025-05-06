#include "mem.h"

dast_bool dast_memeq(const void* lhs, const void* rhs, dast_sz size){
#if DAST_NO_STDLIB
    register const dast_u8 *s1 = (const dast_u8*)lhs;
    register const dast_u8 *s2 = (const dast_u8*)rhs;
    while(size-- > 0) if (*s1++ != *s2++) return dast_false;
    return dast_true;
#else
    return (lhs && rhs) && ((lhs == rhs) || (memcmp(lhs, rhs, size) == 0));
#endif
}

void* dast_memset(void* dest, int ch, dast_sz size){
#if DAST_NO_STDLIB
    register dast_u8 *d = (dast_u8*)dest;
    while(size-- > 0) *d++ = (unsigned char)ch;
    return dest;
#else
    return memset(dest, ch, size);
#endif
}

void* dast_memcpy(void* dest, const void* src, dast_sz size){
#if DAST_NO_STDLIB
    register       dast_u8 *d = (      dast_u8*)dest;
    register const dast_u8 *s = (const dast_u8*)src;
    while(size-- > 0) *d++ = *s++;
    return dest;
#else
    return memcpy(dest, src, size);
#endif
}


void* dast_memmove(void* dest, const void* src, dast_sz size){
#if DAST_NO_STDLIB
    if(dest <= src) return dast_memcpy(dest, src, size);
    register       dast_u8 *d = (      dast_u8*)dest + size - 1;
    register const dast_u8 *s = (const dast_u8*)src + size - 1;
    while(size-- > 0) *d-- = *s--;
    return dest;
#else
    return memmove(dest, src, size);
#endif
}
