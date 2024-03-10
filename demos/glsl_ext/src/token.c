#include "token.h"


const char* token_get_type_string(int type){
	switch(type){
		case TOKEN_PP_INCLUDE:
			return "pp-include";
		case TOKEN_PP_SHADERTYPE:
			return "pp-shadertype";
		case TOKEN_STRING:
			return "string-literal";
		default:
			return "";
	}
}