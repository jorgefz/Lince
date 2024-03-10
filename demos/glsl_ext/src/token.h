#ifndef LINCE_GLSL_EXT_TOKEN_H
#define LINCE_GLSL_EXT_TOKEN_H

#include <ctype.h>

enum token_type	{
	TOKEN_NONE = 0,
	TOKEN_PP_INCLUDE,
	TOKEN_PP_SHADERTYPE,
	TOKEN_STRING,
	TOKEN_IDENTIFIER
};

struct token {
	int type;
	int line;
	size_t location;
	char lexeme[256];
	size_t length;
};

const char* token_get_type_string(int type);

#endif /* LINCE_GLSL_EXT_TOKEN_H */