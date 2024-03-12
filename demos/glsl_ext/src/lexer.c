#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "token.h"

static size_t lexer_get_line_length(const char* str){
	const char* p = str;
	while(*(p++) != '\0'){
		if(*p == '\n') break;
	}
	return p - str;
}

static const char* lexer_get_line_start(const char* p, const char* orig){
	while(p-- >= orig){
		if(*p == '\n') break;
	}
	return p+1;
}

static void lexer_init(struct lexer* lex, const char* source, array_t* tokens){
	hashmap_init(&lex->keywords, 10);
	hashmap_set(&lex->keywords, "include", (void*)(size_t)TOKEN_PP_INCLUDE);
	hashmap_set(&lex->keywords, "type",    (void*)(size_t)TOKEN_PP_SHADERTYPE);

	lex->source = source;
	lex->tokens = tokens;
	lex->p = lex->source;
	lex->length = strlen(source);
	lex->line = 0;
	lex->error = LEX_ERR_OK;
}

static int lexer_end(struct lexer* lex){
	if(lex->error != LEX_ERR_OK) return 1;
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
		.location = loc - lex->source,
		.length = length
	};
	memcpy(tok.lexeme, loc, length);
	tok.lexeme[length] = '\0';
	array_push_back(lex->tokens, &tok);
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
		lex->error = LEX_ERR_UNCLOSED_COMMENT_BLOCK;
		lex->line = line; // Tells which line comment started
	}
}

static void lexer_read_string(struct lexer* lex){
	const char* start = lex->p;

    lexer_add_token(lex, TOKEN_QUOTE, start-1, 1);

	while(lexer_peek(lex) != '\"' && !lexer_end(lex)){

		// Escaped quote
		if(lexer_peek(lex) == '\\' && lexer_peek_next(lex) == '\"'){
			lexer_advance(lex);
		}

		// End of line
		if(lexer_peek(lex) == '\n'){
			lex->error = LEX_ERR_UNTERMINATED_STRING;
			return;
		}

		lexer_advance(lex);
	}
	if(lexer_end(lex)){
		lex->error = LEX_ERR_UNTERMINATED_STRING;
		return;
	}
	lexer_add_token(lex, TOKEN_STRING, start, lex->p - start);
    lexer_add_token(lex, TOKEN_QUOTE, lex->p, 1);
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

    // Read directive name
    const char* start = lex->p;
	while(lexer_is_alphanum(lexer_peek(lex))){
		lexer_advance(lex);
	}

    // TODO: bounds check
    static char directive[TOKEN_LEXEME_MAX] = {0};
    memcpy(directive, start, lex->p - start);
    directive[lex->p - start] = '\0';
    
    if(hashmap_has_key(&lex->keywords, directive)){
        int type = (int)(size_t)hashmap_get(&lex->keywords, directive);
        lexer_add_token(lex, TOKEN_HASH, start-1, 1);
        lexer_add_token(lex, type, start, lex->p - start);
    }
}

const char* lexer_get_error_descr(int err){
	switch(err){
		case LEX_ERR_UNTERMINATED_STRING:
			return "Unterminated string literal";
		case LEX_ERR_UNCLOSED_COMMENT_BLOCK:
			return "Unclosed comment block";
		default:
			return "Unknown error";
	}
}


int lexer_find_tokens(const char* src, array_t* tokens, char* error_string, size_t error_string_max){
	
	struct lexer lex;
	lexer_init(&lex, src, tokens);

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
	lexer_add_token(&lex, TOKEN_NONE, lex.source+lex.length, 0);
	
	if(lex.error != LEX_ERR_OK){
		if(error_string){
			const char* line = lexer_get_line_start(lex.p, lex.source);
			snprintf(error_string, error_string_max,
				"Error on line %d: %s.\n    %d | %.*s\n",
				(int)lex.line,
				lexer_get_error_descr(lex.error),
				(int)lex.line,
				(int)lexer_get_line_length(line),
				line
			);
		}
		return lex.error;
	}

	return lex.error;
}
