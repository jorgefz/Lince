#ifndef LINCE_GLSL_EXT_LEXER_H
#define LINCE_GLSL_EXT_LEXER_H

#include "lince/containers/array.h"
#include "lince/containers/hashmap.h"
#include <ctype.h>

#define LEX_STR_MAX 100

enum lexer_error {
	LEX_ERR_OK = 0,
	LEX_ERR_UNTERMINATED_STRING,
	LEX_ERR_UNCLOSED_COMMENT_BLOCK
};

struct lexer {
	array_t* tokens;		// Token list
	const char* source; // Source code
	const char* p;      // Current pointer
	size_t length;      // Source length
	int line;           // Current line
	hashmap_t keywords; // Pre-defined keywords
	int error;          // Error code
	char error_string[LEX_STR_MAX];
	size_t error_string_length;
};

struct lexer* lexer_init(const char* source, size_t source_length, array_t* tokens);

void lexer_free(struct lexer* lex);

int lexer_find_tokens(struct lexer* lex);

const char* lexer_get_error_descr(int err);

#endif /* LINCE_GLSL_EXT_LEXER_H */