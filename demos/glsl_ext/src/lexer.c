#include <stdio.h>
#include <stdlib.h>
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
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static int lexer_is_numeric(char c){
	return (c >= '0' && c <= '9');
}

static int lexer_is_alphanum(char c){
	return lexer_is_numeric(c) || lexer_is_alphabetic(c);
}

static int lexer_is_valid_name(char c){
	return lexer_is_alphanum(c) || c == '_';
}

static int lexer_is_valid_name_start(char c){
	return lexer_is_alphabetic(c) || c == '_';
}

static void lexer_read_identifier(struct lexer* lex){
	// Seek the beginning of the identifier
	while(!lexer_end(lex)){
		char c = lexer_peek(lex);
		if(lexer_is_valid_name_start(c)){
			break;
		} else if (c == '\n'){
			lex->error = LEX_ERR_EXPECTED_IDENTIFIER;
			return;
		} else if (!isspace(c)){
			lex->error = LEX_ERR_INVALID_IDENTIFIER;
			return;
		}
		lexer_advance(lex);
	}
	if(lexer_end(lex)){
		lex->error = LEX_ERR_EXPECTED_IDENTIFIER;
		return;
	}

	// Read identifier
	const char* start = lex->p;
	while(lexer_is_valid_name(lexer_peek(lex))){
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
    
    if(!hashmap_has_key(&lex->keywords, directive)){
		// Other preprocessor directive, e.g #define
		// GPU GLSL compiler will take care of it
		return;
	}

	int type = (int)(size_t)hashmap_get(&lex->keywords, directive);
	lexer_add_token(lex, TOKEN_HASH, start-1, 1);
	lexer_add_token(lex, type, start, lex->p - start);

	// Read shader type name if relevant
	if(type == TOKEN_PP_SHADERTYPE){
		lexer_read_identifier(lex);
		// if(lex->error != LEX_ERROR_OK) return;
	}
   
}

const char* lexer_get_error_descr(int err){
	switch(err){
		case LEX_ERR_EXPECTED_IDENTIFIER:
			return "Expected identifier";
		case LEX_ERR_INVALID_IDENTIFIER:
			return "Invalid identifier";
		case LEX_ERR_UNTERMINATED_STRING:
			return "Unterminated string literal";
		case LEX_ERR_UNCLOSED_COMMENT_BLOCK:
			return "Unclosed comment block";
		default:
			return "Unknown error";
	}
}


int lexer_find_tokens(struct lexer* lex){
	
	while(!lexer_end(lex)){
		char c = lexer_advance(lex);
		switch(c){

			case '/': // Comments
				if(lexer_match(lex, '/')){
					lexer_seek_next(lex, '\n');
					lex->line++;
					break;
				}
				if(lexer_match(lex, '*')){
					lexer_consume_comment_block(lex);
					break;
				}
				break;
			
			case '#': // Preprocessor directives
				lexer_read_pp_directive(lex);
				break;

			case '\"': // String literals
				lexer_read_string(lex);
				break;

			case '\n': // New line
				lex->line++;
				break;

			default: // Ignore all other tokens
				break;
		}
	}

	lexer_add_token(lex, TOKEN_NONE, lex->source + lex->length, 0);
	
	if(lex->error != LEX_ERR_OK){
		const char* line = lexer_get_line_start(lex->p, lex->source);
		int n = snprintf(lex->error_string, LEX_STR_MAX,
			"Lexer error on line %d: %s.\n    %d | %.*s\n",
			(int)lex->line,
			lexer_get_error_descr(lex->error),
			(int)lex->line,
			(int)lexer_get_line_length(line),
			line
		);
		lex->error_string_length = (size_t)n;
		return lex->error;
	}

	return lex->error;
}



struct lexer* lexer_init(const char* source, size_t source_length, array_t* tokens){
	
	struct lexer* lex = calloc(1, sizeof(struct lexer));
	if(!lex) return NULL;

	lex->source = source;
	lex->p      = lex->source;
	lex->length = source_length;
	lex->tokens = tokens;
	lex->line   = 1;
	lex->error  = LEX_ERR_OK;

	void* res = hashmap_init(&lex->keywords, 10);
	if(!res){
		free(lex);
		return NULL;
	}

	hashmap_set(&lex->keywords, "include", (void*)(size_t)TOKEN_PP_INCLUDE);
	hashmap_set(&lex->keywords, "type",    (void*)(size_t)TOKEN_PP_SHADERTYPE);
	
	return lex;
}



void lexer_free(struct lexer* lex){
	if(!lex) return;
	hashmap_uninit(&lex->keywords);
	free(lex);
}

