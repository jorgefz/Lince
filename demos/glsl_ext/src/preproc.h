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
	PP_ERR_BAD_INCLUDE
};

enum pp_output {
	PP_OUTPUT_VERTEX,
	PP_OUTPUT_FRAGMENT
};

typedef void (*pp_write_fn)(const char* from, size_t length, void* user_data);

void* pp_init(char* source, size_t source_length, hashmap_t* headers, pp_write_fn write_callback, void* user_data);

int pp_run_includes(void* pp);

void pp_free(void* pp);

const char* pp_get_error_string(void* pp);

#endif /* LINCE_GLSL_EXT_PREPROC_H */