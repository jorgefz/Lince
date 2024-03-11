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
#include "preproc.h"

char* Header = (
	"\n"
	"#define macro\n"
	"\n"
	"int header(){\n"
	"    return 0;\n"
	"}\n"
);

char* Source = (
	"#type \"vertex\"\n"
	"#version 450 core\n"
	"\"#include string\";\n"
	"\"\\\"Escaped quotes within string\\\"\";\n"
	"// #include comment\n"
	"/* #include comment \n"
	"continues in the next line*/\n"
	"#include \"header\"\n"
	"void main(){\n"
	"#include\n"
	"}\n"
);


int main() {

	printf("======= TOKENS =======\n");
	array_t tokens;
	int err = lexer_find_tokens(Source, &tokens);

	if(err != 0) return err;

	struct token* tok;
	for(tok = tokens.begin; tok != tokens.end; tok++){
		printf("Token '%-10s': type '%-15s', line %d, loc %d, len %d\n",
			tok->lexeme, token_get_type_string(tok->type),
			tok->line, (int)tok->location, (int)tok->length);
	}


	printf("\n======= PREPROCESSOR =======\n");
	hashmap_t headers;
	hashmap_init(&headers, 10);
	hashmap_set(&headers, "header", Header);

	char* output = calloc(1, 1000);

	err = pp_run_includes(Source, output, &tokens, &headers);

	if(err != 0) return 0;
	
	printf("%s", output);

	array_uninit(&tokens);
	hashmap_uninit(&headers);
	free(output);

	return 0;
}