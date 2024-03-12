#include "preproc.h"
#include "token.h"
#include "lexer.h"

#define PP_STR_MAX 100

struct preproc {
	const char* source;
	const char* psrc;
	
	pp_write_fn write_callback;
	void* user_data;

	hashmap_t* headers;
	array_t* tokens;
	struct token* tok;
	
	int error;
	char error_string[PP_STR_MAX];
};


static const char* pp_get_error_descr(int err){
	switch(err){
		case PP_ERR_UNTERMINATED_STRING:
			return lexer_get_error_descr(LEX_ERR_UNTERMINATED_STRING);
		case PP_ERR_UNCLOSED_COMMENT_BLOCK:
			return lexer_get_error_descr(PP_ERR_UNCLOSED_COMMENT_BLOCK);
		case PP_ERR_NO_HEADER:
			return "Could not find header";
		case PP_ERR_BAD_INCLUDE:
			return "Missing include target";
		default: return "Unknown error";
	}
}

const char* pp_get_error_string(void* _pp){
	struct preproc* pp = _pp;
	return pp->error_string;
}

static size_t pp_get_line_length(const char* str){
	const char* p = str;
	while(*(p++) != '\0'){
		if(*p == '\n') break;
	}
	return p - str;
}

static void pp_println(const char* str){
	const char* p = str;
	while(*(p++) != '\0'){
		if(*p == '\n') break;
	}
	printf("%.*s", (int)(p-str), str);
}

static void pp_write(struct preproc* pp, const char* from, size_t length){
	if(pp->write_callback){
		pp->write_callback(from, length, pp->user_data);
	}
}
static void pp_include(struct preproc* pp){
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
	pp_write(pp, header_source, header_length);
	pp->error = PP_ERR_OK;
}

static void pp_copy_non_tokenized(struct preproc* pp){
	const char* end = pp->source + pp->tok->location;
	pp_write(pp, pp->psrc, end - pp->psrc);
	pp->psrc = end + pp->tok->length;
}


int pp_run_includes(void* _pp){
	struct preproc* pp = _pp;

	// Fetch tokens
	int err = lexer_find_tokens(pp->source, pp->tokens, pp->error_string, PP_STR_MAX);
	if(err != LEX_ERR_OK){
		return err;
	}
	
	// Process tokens
	for(pp->tok = pp->tokens->begin; pp->tok != pp->tokens->end; ++pp->tok){

		pp_copy_non_tokenized(pp);

		if(pp->tok->type == TOKEN_NONE) break;

		switch(pp->tok->type){
			case TOKEN_PP_INCLUDE:
				pp_include(pp);
				break;
			case TOKEN_STRING: break;
			case TOKEN_PP_SHADERTYPE: break;
			default:
				break;
		}

		if(pp->error != PP_ERR_OK) break;
	}

	if(pp->error != PP_ERR_OK){
		snprintf(pp->error_string, PP_STR_MAX,
			"Error on line %d: %s.\n    %d | %.*s\n",
			pp->tok->line, 
			pp_get_error_descr(pp->error),
			pp->tok->line,
			(int)pp_get_line_length(pp->source + pp->tok->location - 1),
			pp->source + pp->tok->location - 1
		);
		return pp->error;
	}

	return pp->error;
}

void* pp_init(char* source, hashmap_t* headers, pp_write_fn write_callback, void* user_data){
	struct preproc* pp = malloc(sizeof(struct preproc));
	if(!pp) return NULL;

	*pp = (struct preproc){
		.source = source,
		.psrc = source,
		.write_callback = write_callback,
		.user_data = user_data,
		.headers = headers,
		.tokens = NULL,
		.tok = NULL,
		.error = PP_ERR_OK,
		.error_string = {0}
	};

	pp->tokens = malloc(sizeof(struct token));
	if(!pp->tokens){
		free(pp);	
		return NULL;
	}
	array_init(pp->tokens, sizeof(struct token));

	return pp;
}

void pp_free(void* _pp){
	struct preproc* pp = _pp;
	if(!pp) return;
	if(pp->tokens){
		array_uninit(pp->tokens);
		free(pp->tokens);
	}
	free(pp);
}



