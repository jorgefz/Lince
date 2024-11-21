#include "str.h"
#include <stdlib.h>

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
    s.str = malloc(len + 1);

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
    s.str = malloc(len + 1);

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

/** @brief Frees character array in string container. Sets `str` to NULL and `len` to zero
 * @param str string container to free.
 */
void string_free(string_t* str){
    if (!str) return;
    str->len = 0;
    if(str->str) free(str->str);
    str->str = NULL;
}
