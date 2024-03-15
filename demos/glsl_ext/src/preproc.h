/** @file preproc.h
 * 
 * Preprocessor for GLSL extensions.
 * 
 * Extensions:
 * 	1. Include directive
 * 		- Insert other text files at the specified location
 * 		- The file path must be provided as a string literal
 * 		- Example: #include "header"
 * 	2. Shader type
 * 		- Specifies the stage of the shader pipeline the code below belongs to
 * 		- Allowed keywords: header, vertex, and fragment.
 * 		- The default type is 'header'.
 * 		- Ignores preprocessor defines and conditionals,
 * 			e.g. the type will always be read in the code below
 * 				#ifdef UNDEFINED_MACRO
 * 				#type vertex
 * 				#endif
 * 		- Example: #type vertex
 *  3. String literals
 * 		- Text enclosed between quotes.
 * 		- Only string literals after the include directive have an effect
 * 		- All other string literals are removed by the preprocessor
 * 
*/


#ifndef LINCE_GLSL_EXT_PREPROC_H
#define LINCE_GLSL_EXT_PREPROC_H


#include <stdio.h>
#include <lince.h>

#include "lince/containers/array.h"
#include "lince/containers/hashmap.h"

enum pp_error {
	PP_ERR_OK = 0,
	PP_ERR_UNTERMINATED_STRING,
	PP_ERR_UNCLOSED_COMMENT_BLOCK,
	PP_ERR_NO_HEADER,
	PP_ERR_BAD_INCLUDE,
	PP_BAD_SHADER_TYPE
};

enum pp_output {
	PP_SHADER_NONE = 0,
	PP_SHADER_HEADER = PP_SHADER_NONE,
	PP_SHADER_VERTEX,
	PP_SHADER_FRAGMENT
};

typedef void (*pp_write_fn)(const char* from, size_t length, int shader_type, void* user_data);

void* pp_init(char* source, size_t source_length, hashmap_t* headers, pp_write_fn write_callback, void* user_data);

int pp_run(void* pp);

void pp_free(void* pp);

const char* pp_get_error_string(void* pp);

#endif /* LINCE_GLSL_EXT_PREPROC_H */