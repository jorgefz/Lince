/*
	GLSL Extended Lexer

	Features
	- String literals
	- Include directive
	- Shader type directive

	Not Supported
	- Escaped quotes within strings, e.g. \"

*/

#include <stdio.h>
#include <lince.h>

#include "token.h"
#include "lexer.h"


char* Header = (
	"int header(){\n"
	"	return 0;\n"
	"}\n"
);

char* Source = (
	"#type \"vertex\"\n"
	"#version 450 core\n"
	"\"#include string\";\n"
	"\"\\\"Escaped quotes within string\\\"\";\n"
	"// #include comment\n"
	"/* #include comment */\n"
	"#include \"header\"\n"
	"\n"
	"void main(){\n"
	"\n"
	"}\n"
);


int main() {

	array_t tokens;
	int success = lexer_find_tokens(Source, &tokens);
	if(success){
		struct token* tok;
		for(tok = tokens.begin; tok != tokens.end; tok++){
			printf("Token '%s': type '%s', line %d\n",
				tok->lexeme, token_get_type_string(tok->type), tok->line);
		}
	}


	return 0;
}