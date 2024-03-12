#ifndef LINCE_GLSL_EXT_LEXER_H
#define LINCE_GLSL_EXT_LEXER_H

#include "lince/containers/array.h"
#include "lince/containers/hashmap.h"
#include <ctype.h>

enum lexer_error {
	LEX_ERR_OK = 0,
	LEX_ERR_UNTERMINATED_STRING,
	LEX_ERR_UNCLOSED_COMMENT_BLOCK
};

struct lexer {
	array_t tokens;		// Token list
	const char* source; // Source code
	const char* p;      // Current pointer
	size_t length;      // Source length
	int line;           // Current line
	int error;          // Error code
	hashmap_t keywords; // Pre-defined keywords
};

int lexer_find_tokens(const char* src, array_t* tokens, char* error_string, size_t error_string_max);

const char* lexer_get_error_descr(int err);

#endif /* LINCE_GLSL_EXT_LEXER_H */