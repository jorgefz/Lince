#ifndef LINCE_GLSL_EXT_PREPROC_H
#define LINCE_GLSL_EXT_PREPROC_H


#include <stdio.h>
#include <lince.h>

#include "lince/containers/array.h"
#include "lince/containers/hashmap.h"

#include "token.h"
#include "lexer.h"


enum pp_error {
	PP_ERR_OK = LEX_ERR_OK,
	PP_ERR_NO_HEADER,
	PP_ERR_BAD_INCLUDE
};

enum pp_output {
	PP_OUTPUT_VERTEX,
	PP_OUTPUT_FRAGMENT
};

typedef void (*pp_write_fn)(const char* from, size_t length, void* user_data);

struct preproc {
	const char* source;
	const char* psrc;
	
	pp_write_fn write_callback;
	void* user_data;

	hashmap_t* headers;
	array_t* tokens;
	int error;
	struct token* tok;
};


int pp_run_includes(struct preproc* pp);

struct preproc* pp_init(char* source, hashmap_t* headers, pp_write_fn write_callback, void* user_data);

void pp_free(struct preproc* pp);

const char* pp_get_error_string(int err);

#endif /* LINCE_GLSL_EXT_PREPROC_H */