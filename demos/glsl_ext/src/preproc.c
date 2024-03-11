#include "preproc.h"

const char* pp_get_error_string(int err){
	switch(err){
		case PP_ERR_NO_HEADER: return "Could not find header";
		case PP_ERR_BAD_INCLUDE: return "Missing include target";
		default: return "Unknown error";
	}
}

static void pp_putsn(const char* str, size_t n){
	printf("%.*s", (int)n, str);
}

static void pp_println(const char* str){
	const char* p = str;
	while(*(p++) != '\0'){
		if(*p == '\n') break;
	}
	printf("%.*s", (int)(p-str), str);
}

void pp_include(struct preproc* pp){
	if(pp->tok->type != TOKEN_PP_INCLUDE){
		pp->error = PP_ERR_BAD_INCLUDE;
		return;
	}

	struct token* quote = pp->tok + 1;
	if(quote->type != TOKEN_QUOTE){
		pp->error = PP_ERR_BAD_INCLUDE;
		return;
	}

	struct token* target = pp->tok + 2;
	if(target->type != TOKEN_STRING){
		pp->error = PP_ERR_BAD_INCLUDE;
		return;
	}

	char* header_source = hashmap_get(pp->headers, target->lexeme);
	if(!header_source){
		pp->error = PP_ERR_NO_HEADER;
		return;
	}

	size_t header_length = strlen(header_source);
	memcpy(pp->pout, header_source, header_length);
	pp->pout += header_length;
	pp->error = PP_ERR_OK;
}

void pp_copy_non_tokenized(struct preproc* pp){
	const char* end = pp->source + pp->tok->location;
	memcpy(pp->pout, pp->psrc, end-pp->psrc);
	pp->pout += end - pp->psrc;
	pp->psrc = end + pp->tok->length;
}

int pp_run_includes(const char* source, char* output, array_t* tokens, hashmap_t* headers){

	struct preproc pp = {
		.source = source,
		.output = output,
		.pout = output,
		.psrc = source,
		.tok = NULL,
		.error = PP_ERR_OK,
		.headers = headers,
		.tokens = tokens
	};
	
	// Header pass
	for(pp.tok = pp.tokens->begin; pp.tok != pp.tokens->end; ++pp.tok){

		pp_copy_non_tokenized(&pp);

		if(pp.tok->type == TOKEN_NONE) break;

		switch(pp.tok->type){
			case TOKEN_PP_INCLUDE:
				pp_include(&pp);
				break;
			case TOKEN_STRING: break;
			case TOKEN_PP_SHADERTYPE: break;
			default:
				break;
		}

		if(pp.error != PP_ERR_OK) break;
	}

	if(pp.error != PP_ERR_OK){
		printf("GLSL-EXT: Error on line %d: %s\n",
			pp.tok->line, pp_get_error_string(pp.error));
		printf(" -> ");
		pp_println(pp.source + pp.tok->location - 1);
		return pp.error;
	}

	return pp.error;
}
