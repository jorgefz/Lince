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
	"#include\"\n"
	"}\n"
);


void write_callback(const char* from, size_t length, void* data){
	char* output = *(char**)data;
	memcpy(output, from, length);
	*(char**)data = output + length;
	// printf("%.*s", (int)length, from);
}

int main() {

	hashmap_t headers;
	hashmap_init(&headers, 10);
	hashmap_set(&headers, "header", Header);
	char* output = calloc(1, 1000);
	char* pout = output;

	void* pp = pp_init(Source, &headers, write_callback, &pout);
	if(!pp) return -1;

	int err = pp_run_includes(pp);
	if(err != 0){
		printf("[GLSL-EXT] %s", pp_get_error_string(pp));
	}
	pp_free(pp);

	printf("%s", output);

	free(output);

	return 0;
}