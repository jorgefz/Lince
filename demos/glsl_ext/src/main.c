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
#include <string.h>
#include <ctype.h>
#include <lince.h>

enum token_type	{
	TOKEN_NONE = 0,
	TOKEN_PP_INCLUDE,
	TOKEN_PP_SHADERTYPE,
	TOKEN_STRING,
	TOKEN_IDENTIFIER
};

struct token {
	int type;
	int line;
	size_t location;
	char lexeme[256];
	size_t length;
};

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

enum lexer_error {
	ERR_OK = 0,
	ERR_UNFINISHED_STRING,
	ERR_UNCLOSED_COMMENT_BLOCK
};

struct lexer {
	array_t tokens;		// Token list
	const char* source; // Source code
	const char* p;      // Current pointer
	size_t length;      // Source length
	int line;           // Current line
	int error;          // Error code
	hashmap_t keywords; // Pre-defined keywords
};

void lexer_init(struct lexer* lex, const char* source){
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

int lexer_end(struct lexer* lex){
	if(lex->error != ERR_OK) return 1;
	return (lex->p >= lex->source + lex->length);
}

char lexer_peek(struct lexer* lex){
	return *(lex->p);
}

char lexer_peek_next(struct lexer* lex){
	return *(lex->p+1);
}

char lexer_advance(struct lexer* lex){
	return *(lex->p++);
}

int lexer_match(struct lexer* lex, char c){
	if(lexer_end(lex)) return 0;
	if(*lex->p != c) return 0;
	lex->p++;
	return 1;
}


void lexer_seek_next(struct lexer* lex, char c){
	while(lexer_peek(lex) != c && !lexer_end(lex)){
		if(lexer_peek(lex) == '\n') lex->line++;
		lexer_advance(lex);
	}
}

void lexer_add_token(struct lexer* lex, int type, const char* loc, size_t length){
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

void lexer_consume_comment_block(struct lexer* lex){
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

void lexer_read_string(struct lexer* lex){
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

int lexer_is_alphabetic(char c){
	return (c >= 'A' && c <= 'z');
}

int lexer_is_numeric(char c){
	return (c >= '0' && c <= '9');
}

int lexer_is_alphanum(char c){
	return lexer_is_numeric(c) || lexer_is_alphabetic(c);
}

void lexer_read_identifier(struct lexer* lex){
	const char* start = lex->p;
	while(lexer_is_alphanum(lexer_peek(lex))){
		lexer_advance(lex);
	}
	lexer_add_token(lex, TOKEN_IDENTIFIER, start, lex->p - start);
}

void lexer_read_pp_directive(struct lexer* lex){
	lexer_read_identifier(lex);
	struct token* tok = array_back(&lex->tokens);
	if(hashmap_has_key(&lex->keywords, tok->lexeme)){
		tok->type = (int)(size_t)hashmap_get(&lex->keywords, tok->lexeme);
	} else {
		array_pop_back(&lex->tokens);
	}
}

const char* lexer_get_error_string(enum lexer_error err){
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

	if(lex.error != ERR_OK){
		printf("Error on line %d: %s", lex.line, lexer_get_error_string(lex.error));
		return 0;
	}

	*tokens = lex.tokens;
	hashmap_uninit(&lex.keywords);
	return 1;
}


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