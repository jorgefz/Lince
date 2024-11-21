#ifndef STR_H
#define STR_H

/** @struct string_t 
* @brief Container for a character array and its length.
* Lightweight (16 bytes on 64-bit systems) and easy to use.
* Character array is stored in the heap.
* 
* By default, and in order to be supported by exising C conventions,
* the last character in the buffer `str` will be a null-terminator character,
* and the number of bytes `len` will not take into it account.
* */
typedef struct string {
    char* str;  ///< Character array
    size_t len; ///< Number of characters in the array
} string_t;

/** @brief Creates string from a string literal
 * @param LIT charcter array literal
 * @returns string container storing copy of string literal
*/
#define string_from_literal(LIT) string_from_cstr((LIT), sizeof(LIT))

/** @brief Creates a string from a character array
 * @param str Character array 
 * @param len Number of chacaters in the character array
 * @return string container storing copy of input character array
*/
string_t string_from_chars(const char* chars, size_t len);

/** @brief Create an empty string with given size
 * @param len Number of characters in the string
 * @return string container storing a string of `(char)0` of length `len`
*/
string_t string_from_len(size_t len);

/** @brief Frees character array in string container. Sets `str` to NULL and `len` to zero */
void string_free(string_t* str);




#endif /* STR_H */