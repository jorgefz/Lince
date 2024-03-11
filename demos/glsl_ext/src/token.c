#include "token.h"


const char* token_get_type_string(int type){
	switch(type){
		case TOKEN_PP_INCLUDE:
			return "pp-include";
		case TOKEN_PP_SHADERTYPE:
			return "pp-shadertype";
		case TOKEN_STRING:
			return "string-literal";
		case TOKEN_QUOTE:
			return "string-quote";
		case TOKEN_HASH:
			return "hash";
		case TOKEN_IDENTIFIER:
			return "identifier";
		default:
			return "";
	}
}