#ifndef DAST_STR_H
#define DAST_STR_H

#include "mem.h"

/** @struct string_t 
* @brief Container for a character array and its length.
* Lightweight (16 bytes on 64-bit systems) and easy to use.
* Character array is stored in the heap.
* 
* By default, and in order to be supported by exising C conventions,
* the last character in the buffer `str` will be a null-terminator character,
* and the number of bytes `len` will not take it into account.
* */
typedef struct dast_string {
    char* str;   /**< Character array                    */
    dast_sz len; /**< Number of characters in the array  */
} string_t;

/** @brief Resolves to `dast_true` if a string was successfully initialised */
#define string_ok(STR) (dast_bool)((STR).str)

/** @brief Returns the allocator used by a string.
 * @note Attempting to get the allocator of a scoped string results in undefined behaviour.
*/
#define string_get_alloc(S) ((S).str ? ((dast_allocator_t*)((S).str)-1) : dast_null)

/** @brief Creates a string_t from a string literal valid for the current scope.
 *  @note no copy of the string data is made, `str` will simply point to the input character array.
 *  @note do not call `string_free` on the result of this macro.
*/
#define string_scoped_lit(LIT) (string_t){.str = (char*)(LIT), .len = (sizeof(LIT)-1)}

/** @brief Creates a string_t from a character array valid for the current scope
 *  @note no copy of the string data is made, `str` will simply point to the input character array.
 *  @note do not call `string_free` on the result of this macro.
*/
#define string_scoped(PTR, LEN) (string_t){.str = (char*)(PTR), .len = (LEN)}

/** @brief Creates string from a string literal using a custom allocator
 * @param LIT character array literal
 * @param ALLOC Custom allocator
 * @returns string container storing copy of string literal
*/
#define string_from_literal_custom(LIT, ALLOC) string_from_chars_custom((LIT), sizeof(LIT)-1, (ALLOC))

/** @brief Creates string from a string literal
 * @param LIT character array literal
 * @returns string container storing copy of string literal
*/
#define string_from_literal(LIT) string_from_chars((LIT), sizeof(LIT)-1)

/** @brief Creates a string from a character array, using a custom allocator
 * @param str Character array 
 * @param len Number of chacaters in the character array
 * @param alloc Custom allocation functions
 * @return string container storing copy of input character array
*/
string_t string_from_chars_custom(const char* chars, dast_sz len, dast_allocator_t alloc);

/** @brief Creates a string from a character array
 * @param str Character array 
 * @param len Number of chacaters in the character array
 * @return string container storing copy of input character array
*/
string_t string_from_chars(const char* chars, dast_sz len);

/** @brief Create an empty string with given size, using a custom allocator
 * @param len Number of characters in the string
 * @param alloc Custom allocation functions
 * @return string container storing a string of `(char)0` of length `len`
*/
string_t string_from_len_custom(dast_sz len, dast_allocator_t alloc);

/** @brief Create an empty string with given size
 * @param len Number of characters in the string
 * @return string container storing a string of `(char)0` of length `len`
*/
string_t string_from_len(dast_sz len);

/** @brief Create a string from a printf-style format char array, using a custom allocator.
 * @param alloc Custom allocation functions
 * @param fmt Printf-style format string
 * @param args Arguments for string format
 * @returns string with arguments composed following input format
 * @note When using %s (string) arguments on formatted strings,
 * it is recommended to also specify the maximum number of characters to write:
 *      string_from_fmt("%.*s", len, str);
 * @warning When DAST_NO_STDLIB is defined, this function will always return an invalid string!
**/
string_t string_from_fmt_custom(dast_allocator_t alloc, const char fmt[], ...);

/** @brief Create a string from a format char array.
 * @param fmt Format string
 * @param args Arguments for string format 
 * @returns string with arguments composed following input format
 * @note When using %s (string) arguments on formatted strings,
 * it is recommended to also specify the maximum number of characters to write:
 *      string_from_fmt("%.*s", len, str);
 * @warning When DAST_NO_STDLIB is defined, this function will always return an invalid string!
**/
string_t string_from_fmt(const char fmt[], ...);

/** @brief Duplicate an existing string.
 * @param s String to copy.
 * @param alloc Allocator for the new string.
 * @returns New heap-allocated string.
*/
string_t string_copy_custom(string_t s, dast_allocator_t alloc);

/** @brief Duplicate an existing string.
 * @param s String to copy.
 * @param alloc Allocator for the new string.
 * @returns New heap-allocated string.
 * @note Attempting to copy a scoped string results in undefined behaviour.
*/
string_t string_copy(string_t s);

/** @brief Frees character array in string. Sets `str` to NULL and `len` to zero.
 * @param str string to deallocate.
 * @note Attempting to free a scoped string results in undefined behaviour.
 */
void string_free(string_t* str);


#endif /* DAST_STR_H */