#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* Global allocation interface - defaults to std library */
static void* (*string_alloc)(size_t size)   = malloc;
static void  (*string_dealloc)(void* block) = free;


/* Substitute for calloc to use malloc wrapper */
static void* string_calloc(size_t count, size_t size){
	void* block = string_alloc(count * size);
	if(block) memset(block, 0, count * size);
	return block;
}


/* Provide custom allocator functions */
void string_set_alloc(
	void* (*user_alloc)   (size_t size),
	void  (*user_dealloc) (void* block)
) {
	if(user_alloc)   string_alloc   = user_alloc;
	if(user_dealloc) string_dealloc = user_dealloc;
}


/** @brief Creates a string from a character array
 * @param str Character array. If NULL, an empty string is created with given size
 * @param len Number of chacaters in the character array
 * @return string container storing copy of input character array
 * If allocation failed, the `str` field of the returned container will be NULL.
*/
string_t string_from_chars(const char* chars, size_t len){

    if(!chars){
        return string_from_len(len);
    }

    string_t s;
    s.str = string_alloc(len + 1);

    if (!s.str){
        s.len = 0;
        return s;
    }

    for(size_t i = 0; i != len; ++i){
        s.str[i] = chars[i];
    }
    s.str[len] = (char)0;
    s.len = len;

    return s;
}

/** @brief Create an empty string with given size
 * @param len Number of characters in the string
 * @return string container storing a string of `(char)0` of length `len`.
 * If allocation failed, the `str` field of the returned container will be NULL.
*/
string_t string_from_len(size_t len){
    string_t s;
    s.str = string_alloc(len + 1);

    if(!s.str){
        s.len = 0;
        return s;
    }

    s.len = len;
    for(size_t i = 0; i != len + 1; ++i){
        s.str[i] = (char)0;
    }

    return s;
}

string_t string_from_fmt(const char fmt[], ...){
    // Two va_list objects needed:
    //  (1) for calculating the maximum number of bytes written to the buffer
    //  (2) for writing to the buffer
    // Cannot reuse only one since it is consumed after use
    va_list arg1, arg2;
    va_start(arg1, fmt);
    va_copy(arg2, arg1);

    size_t len = vsnprintf(NULL, 0, fmt, arg1); // Fetch expected length without writing to a buffer
    va_end(arg1);

    if(len < 0) return (string_t){0};

    string_t s = string_from_len(len);
    if(!s.str) return s;

    vsnprintf(s.str, s.len + 1, fmt, arg2); // Takes length including null-terminator, but returns length without it
    va_end(arg2);
    
    return s;
}

/** @brief Frees character array in string container. Sets `str` to NULL and `len` to zero
 * @param str string container to free.
 */
void string_free(string_t* str){
    if (!str) return;
    str->len = 0;
    if(str->str) string_dealloc(str->str);
    str->str = NULL;
}
