#include "str.h"



string_t string_from_chars_custom(const char* chars, dast_sz len, dast_allocator_t alloc){
    string_t s = {0};
    if(!alloc.alloc || !alloc.free){
        return (string_t){0};
    }

    /* Reserve space for allocator and char array with terminating character
       Storing the allocator next to the char array saves us from storing it
       in the string_t object, making it lighter. */
    void* block = alloc.alloc( sizeof(char)*(len + 1) + sizeof(dast_allocator_t) );
    if(!block){
        return (string_t){0};
    }

    /* Copy allocator into space before character array */
    dast_allocator_t *ap = block;
    *ap = alloc;
    s.str = (char*)block + sizeof(dast_allocator_t);

    if(chars){
        dast_memcpy(s.str, chars, len);
        s.str[len] = (char)0;
    } else {
        dast_memset(s.str, 0, len + 1);
    }

    s.len = len;
    return s;
}

string_t string_from_chars(const char* chars, dast_sz len){
    return string_from_chars_custom(chars, len, DAST_DEFAULT_ALLOCATOR);
}

string_t string_from_len_custom(dast_sz len, dast_allocator_t alloc){
    return string_from_chars_custom(dast_null, len, alloc);
}

string_t string_from_len(dast_sz len){
    return string_from_len_custom(len, DAST_DEFAULT_ALLOCATOR);
}

string_t string_from_fmt_custom(dast_allocator_t alloc, const char fmt[], ...){
#ifdef DAST_NO_STDLIB
    (void)fmt;
    (void)alloc;
    return (string_t){0};
#else
    int len;
    string_t s;
    
    /** Two va_list objects needed:
     *  (1) for calculating the maximum number of bytes written to the buffer
     *  (2) for writing to the buffer
     * Cannot reuse only one since it is consumed after use
     */
    va_list arg1, arg2;
    va_start(arg1, fmt);
    va_copy(arg2, arg1);

    /* Fetch expected length without writing to a buffer */
    len = vsnprintf(dast_null, 0, fmt, arg1);
    va_end(arg1);

    if(len < 0) return (string_t){0};

    s = string_from_len_custom((dast_sz)len, alloc);
    if(!s.str) return s;

    /* Takes length including null-terminator, but returns length without it */
    vsnprintf(s.str, s.len + 1, fmt, arg2);
    va_end(arg2);
    return s;
#endif
}

string_t string_from_fmt(const char fmt[], ...){
#ifdef DAST_NO_STDLIB
    (void)fmt;
    return (string_t){0};
#else
    /* Must repeat code in string_from_fmt_custom because in C89
     * there is no __VA_ARGS__, so we can't pass the variadic arguments
     * to string_from_fmt_custom
     */
    int len;
    string_t s;
    
    /** Two va_list objects needed:
     *  (1) for calculating the maximum number of bytes written to the buffer
     *  (2) for writing to the buffer
     * Cannot reuse only one since it is consumed after use
     */
    va_list arg1, arg2;
    va_start(arg1, fmt);
    va_copy(arg2, arg1);

    /* Fetch expected length without writing to a buffer */
    len = vsnprintf(dast_null, 0, fmt, arg1);
    va_end(arg1);

    if(len < 0) return (string_t){0};

    s = string_from_len((dast_sz)len);
    if(!s.str) return s;

    /* Takes length including null-terminator, but returns length without it */
    vsnprintf(s.str, s.len + 1, fmt, arg2);
    va_end(arg2);
    return s;
#endif
}

/** @brief Duplicate an existing string.
 * @param s String to copy.
 * @param alloc Allocator for the new string.
 * @returns New heap-allocated string.
*/
string_t string_copy_custom(string_t s, dast_allocator_t alloc){
    if(!alloc.alloc || !alloc.free || !s.str) return (string_t){0};
    return string_from_chars_custom(s.str, s.len, alloc);
}

/** @brief Duplicate an existing string.
 * @param s String to copy.
 * @param alloc Allocator for the new string.
 * @returns New heap-allocated string.
 * @note Attempting to copy a scoped string results in undefined behaviour.
*/
string_t string_copy(string_t s){
    dast_allocator_t* alloc = string_get_alloc(s);
    return string_from_chars_custom(s.str, s.len, *alloc);
}

void string_free(string_t* str){
    if (!str) return;

    if(str->str){
        dast_allocator_t* alloc = string_get_alloc(*str);
        alloc->free(alloc); /* This works because char array is located right after the allocator in memory. */
    }
    
    str->len = 0;
    str->str = dast_null;
}
