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

int lexer_find_tokens(const char* src, array_t* tokens);

const char* lexer_get_error_string(int err);

#endif /* LINCE_GLSL_EXT_LEXER_H */