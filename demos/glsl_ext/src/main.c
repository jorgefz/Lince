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

char Header[] = (
	"\n"
	"#define header_macro\n"
	"\n"
	"int header(){\n"
	"    return 0;\n"
	"}\n"
);

char Source[] = (
	"\n"
	"#version 450 core\n"
	"#include \"header\"\n"
	"\n"
	"#type vertex\n"
	"void main(){}\n"
	"\n"
	"#type fragment\n"
	"void main(){}\n"
);


struct buffers {
	char* vertex;
	char* fragment;
};

void write_callback(const char* from, size_t length, int shader_type, void* data){
	struct buffers* bufs = data;
	switch(shader_type){
		
		case PP_SHADER_VERTEX:
			memcpy(bufs->vertex, from, length);
			bufs->vertex += length;
			break;

		case PP_SHADER_FRAGMENT:
			memcpy(bufs->fragment, from, length);
			bufs->fragment += length;
			break;

		case PP_SHADER_HEADER:
			memcpy(bufs->vertex, from, length);
			bufs->vertex += length;
			memcpy(bufs->fragment, from, length);
			bufs->fragment += length;
	}
}

int main() {

	hashmap_t headers;
	hashmap_init(&headers, 10);
	hashmap_set(&headers, "header", Header);

	char* vertex_shader = calloc(1, 1000);
	char* fragment_shader = calloc(1, 1000);
	struct buffers bufs = {.vertex=vertex_shader, .fragment=fragment_shader};

	struct preproc* pp = pp_init(Source, sizeof(Source), &headers, write_callback, &bufs);
	if(!pp) return -1;

	int err = pp_run(pp);
	if(err != 0){
		printf("[GLSL-EXT] %s", pp_get_error_string(pp));
	}
	pp_free(pp);

	printf("============= VERTEX =============\n");
	printf(vertex_shader);
	printf("\n");

	printf("============= FRAGMENT =============\n");
	printf(fragment_shader);
	printf("\n");

	free(vertex_shader);
	free(fragment_shader);

	return 0;
}