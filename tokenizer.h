#ifndef GUARD_f82d6e5a_6311_11eb_a3e7_507b9dcc3ec
#define GUARD_f82d6e5a_6311_11eb_a3e7_507b9dcc3ec

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    TOK_LEFT_PAREN, TOK_RIGHT_PAREN, TOK_LEFT_BRACE, TOK_RIGHT_BRACE,                      // braces
    TOK_IF, TOK_ELSE, TOK_FOR, TOK_WHILE, TOK_RETURN,                                      // control
    TOK_COMMA, TOK_SEMICOLON,                                                              // seperators
    TOK_DOT, TOK_ARROW,                                                                    // properties
    TOK_PLUS, TOK_MINUS, TOK_SLASH, TOK_STAR, TOK_INC, TOK_DEC,                            // arithmetic
    TOK_AMPERSAND, TOK_OR, TOK_BITOR, TOK_BITXOR, TOK_NOT, TOK_BITNOT,                     // logic
    TOK_ASSIGN, TOK_EQ, TOK_NEQ, TOK_GREATER, TOK_GREATER_EQUAL, TOK_LESS, TOK_LESS_EQUAL, // comparisons
    TOK_IDENTIFIER, TOK_STRING, TOK_CHAR, TOK_NUMBER,                                      // stored in strings
    TOK_TYPEDEF, TOK_STATIC, TOK_CONST, TOK_ENUM, TOK_STRUCT, TOK_PREPROCESSOR_DIRECTIVE   // misc.
} typedef TOKEN_TYPE;

typedef struct {
    TOKEN_TYPE  type;
    char       *content;
    ssize_t     line;
} Token;

// Puts tokens in the provided array, and the length in a pointer len.
// Returns "" if it ran successfuly or a detailed explanation of the error if it failed.
char *tokenize(char *code, Token **tokens, ssize_t *len_ptr);
// Debugging representation of a token.
char *token_to_string(Token t);

#endif
