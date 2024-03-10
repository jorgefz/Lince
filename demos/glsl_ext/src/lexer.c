#include "lexer.h"
#include "token.h"
#include <ctype.h>

static void lexer_init(struct lexer* lex, const char* source){
	array_init(&lex->tokens, sizeof(struct token));
	hashmap_init(&lex->keywords, 10);
	hashmap_set(&lex->keywords, "include", (void*)(size_t)TOKEN_PP_INCLUDE);
	hashmap_set(&lex->keywords, "type",    (void*)(size_t)TOKEN_PP_SHADERTYPE);

	lex->source = source;
	lex->p = lex->source;
	lex->length = strlen(source);
	lex->line = 0;
	lex->error = ERR_OK;
}

static int lexer_end(struct lexer* lex){
	if(lex->error != ERR_OK) return 1;
	return (lex->p >= lex->source + lex->length);
}

static char lexer_peek(struct lexer* lex){
	return *(lex->p);
}

static char lexer_peek_next(struct lexer* lex){
	return *(lex->p+1);
}

static char lexer_advance(struct lexer* lex){
	return *(lex->p++);
}

static int lexer_match(struct lexer* lex, char c){
	if(lexer_end(lex)) return 0;
	if(*lex->p != c) return 0;
	lex->p++;
	return 1;
}

static void lexer_seek_next(struct lexer* lex, char c){
	while(lexer_peek(lex) != c && !lexer_end(lex)){
		if(lexer_peek(lex) == '\n') lex->line++;
		lexer_advance(lex);
	}
}

static void lexer_add_token(struct lexer* lex, int type, const char* loc, size_t length){
	struct token tok = {
		.type = type,
		.line = lex->line,
		.location = lex->p - lex->source,
		.length = length
	};
	memcpy(tok.lexeme, loc, length);
	tok.lexeme[length] = '\0';
	array_push_back(&lex->tokens, &tok);
}

static void lexer_consume_comment_block(struct lexer* lex){
	int line = lex->line;

	while(!lexer_end(lex)
		&& lexer_peek(lex) != '*'
		&& lexer_peek_next(lex) != '/'){
		
		if(lexer_peek(lex) == '\n'){
			lex->line++;
		}

		lexer_advance(lex);
	}

	if(lexer_end(lex)){
		lex->error = ERR_UNCLOSED_COMMENT_BLOCK;
		lex->line = line; // Tells which line comment started
	}
}

static void lexer_read_string(struct lexer* lex){
	const char* start = lex->p;

	while(lexer_peek(lex) != '\"' && !lexer_end(lex)){

		// Escaped quote
		if(lexer_peek(lex) == '\\' && lexer_peek_next(lex) == '\"'){
			lexer_advance(lex);
		}

		// End of line
		if(lexer_peek(lex) == '\n'){
			lex->error = ERR_UNFINISHED_STRING;
			return;
		}

		lexer_advance(lex);
	}
	if(lexer_end(lex)){
		lex->error = ERR_UNFINISHED_STRING;
		return;
	}
	lexer_add_token(lex, TOKEN_STRING, start, lex->p - start);
	lexer_advance(lex); // Consume closing quote
}

static int lexer_is_alphabetic(char c){
	return (c >= 'A' && c <= 'z');
}

static int lexer_is_numeric(char c){
	return (c >= '0' && c <= '9');
}

static int lexer_is_alphanum(char c){
	return lexer_is_numeric(c) || lexer_is_alphabetic(c);
}

static void lexer_read_identifier(struct lexer* lex){
	const char* start = lex->p;
	while(lexer_is_alphanum(lexer_peek(lex))){
		lexer_advance(lex);
	}
	lexer_add_token(lex, TOKEN_IDENTIFIER, start, lex->p - start);
}

static void lexer_read_pp_directive(struct lexer* lex){
	lexer_read_identifier(lex);
	struct token* tok = array_back(&lex->tokens);
	if(hashmap_has_key(&lex->keywords, tok->lexeme)){
		tok->type = (int)(size_t)hashmap_get(&lex->keywords, tok->lexeme);
	} else {
		array_pop_back(&lex->tokens);
	}
}

const char* lexer_get_error_string(int err){
	switch(err){
		case ERR_UNFINISHED_STRING:
			return "Unfinished string";
		case ERR_UNCLOSED_COMMENT_BLOCK:
			return "Unclosed comment block";
		default:
			return "";
	}
}


int lexer_find_tokens(const char* src, array_t* tokens){
	
	struct lexer lex;
	lexer_init(&lex, src);

	while(!lexer_end(&lex)){
		char c = lexer_advance(&lex);
		switch(c){

			case '/': // Comments
				if(lexer_match(&lex, '/')){
					lexer_seek_next(&lex, '\n');
					lex.line++;
					break;
				}
				if(lexer_match(&lex, '*')){
					lexer_consume_comment_block(&lex);
					break;
				}
				break;
			
			case '#': // Preprocessor directives
				lexer_read_pp_directive(&lex);
				break;

			case '\"': // String literals
				lexer_read_string(&lex);
				break;

			case '\n': // New line
				lex.line++;
				break;

			default: // Ignore all other tokens
				break;
		}
	}

	hashmap_uninit(&lex.keywords);
	
    if(lex.error != ERR_OK){
		printf("Error on line %d: %s", lex.line, lexer_get_error_string(lex.error));
		return lex.error;
	}

	*tokens = lex.tokens;
	return 0;
}
