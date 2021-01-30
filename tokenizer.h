#include <stdio.h>
#include <stdlib.h>

enum {
    TOK_LEFT_PAREN, TOK_RIGHT_PAREN, TOK_LEFT_BRACE, TOK_RIGHT_BRACE,                     // braces
    TOK_IF, TOK_ELSE, TOK_FOR, TOK_WHILE, TOK_RETURN,                                     // control
    TOK_COMMA, TOK_SEMICOLON,                                                             // seperators
    TOK_DOT, TOK_ARROW,                                                                   // properties
    TOK_PLUS, TOK_MINUS, TOK_SLASH, TOK_STAR, TOK_INC, TOK_DEC,                           // arithmetic
    TOK_AMPERSAND, TOK_OR, TOK_BITWISE_OR, TOK_BITWISE_XOR, TOK_NOT, TOK_BITWISE_NOT,     // logic
    TOK_EQUAL, TOK_EQUAL_EQUAL, TOK_GREATER, TOK_GREATER_EQUAL, TOK_LESS, TOK_LESS_EQUAL, // comparisons
    TOK_IDENTIFIER, TOK_STRING, TOK_CHAR, TOK_NUMBER,                                     // stored in strings
    TOK_TYPEDEF, TOK_STATIC, TOK_CONST, TOK_ENUM, TOK_STRUCT, TOK_PREPROCESSOR_DIRECTIVE  // misc.
} typedef TOKEN_TYPE;

typedef struct {
    TOKEN_TYPE  type;
    char       *content;
    ssize_t     line;
} Token;

// Puts tokens in the provided array.
// Returns how many tokens there are.
ssize_t tokenize(char *code, Token **tokens);
