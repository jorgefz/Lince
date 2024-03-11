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

struct preproc {
	const char* source;
	char* output;
	size_t output_size;
	size_t output_max;

	const char* psrc;
	char* pout;

	hashmap_t* headers;
	array_t* tokens;
	int error;
	struct token* tok;
};

int pp_run_includes(const char* source, char* output, array_t* tokens, hashmap_t* headers);

const char* pp_get_error_string(int err);

#endif /* LINCE_GLSL_EXT_PREPROC_H */