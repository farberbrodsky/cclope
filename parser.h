#ifndef GUARD_8a64339e_6312_11eb_b301_507b9dcc3ec8
#define GUARD_8a64339e_6312_11eb_b301_507b9dcc3ec8

#include "tokenizer.h"

typedef enum {
    EXPR_POST_INC, EXPR_POST_DEC, EXPR_FUNC_CALL,                   // precidence 1
    EXPR_ARRAY_INDEX, EXPR_DOT, EXPR_ARROW,                         // precidence 1
    EXPR_PRE_INC, EXPR_PRE_DEC, EXPR_UNARY_PLUS, EXPR_UNARY_MINUS,  // precidence 2
    EXPR_NOT, EXPR_BITWISE_NOT, EXPR_DEREF, EXPR_ADDR, EXPR_SIZEOF, // precidence 2
    EXPR_MUL, EXPR_DIV, EXPR_MOD,                                   // precidence 3
    EXPR_ADD, EXPR_SUB,                                             // precidence 4
    EXPR_SHLEFT, EXPR_SHRIGHT,                                      // precidence 5
    EXPR_GREATER, EXPR_GREATER_EQUAL, EXPR_LESS, EXPR_LESS_EQUAL,   // precidence 6
    EXPR_EQ, EXPR_NEQ,                                              // precidence 7
    EXPR_BITAND,                                                    // precidence 8
    EXPR_BITXOR,                                                    // precidence 9
    EXPR_BITOR,                                                     // precidence 10
    EXPR_AND,                                                       // precidence 11
    EXPR_OR,                                                        // precidence 12
    EXPR_TERNARY,                                                   // precidence 13
    EXPR_ASSIGN,        EXPR_ASSIGN_ADD,     EXPR_ASSIGN_SUB,       // precidence 14
    EXPR_ASSIGN_MUL,    EXPR_ASSIGN_DIV,     EXPR_ASSIGN_MOD,       // precidence 14
    EXPR_ASSIGN_SHLEFT, EXPR_ASSIGN_SHRIGHT,                        // precidence 14
    EXPR_ASSIGN_BITAND, EXPR_ASSIGN_BITXOR,  EXPR_ASSIGN_BITOR,     // precidence 14
    EXPR_COMMA                                                      // precidence 15
} EXPR_TYPE;

typedef struct {
    EXPR_TYPE type;
    void *data;
} Expr;

// ExprTwo is used for binary operators, like ADD and MUL.
typedef struct {
    Expr a;
    Expr b;
} ExprTwo;

// Moves tokens pointer in-place to where the expression ends.
// Returns "" if it parsed successfuly or a detailed explanation of the error if it failed.
char *parse_expr(Token **tokens, Expr *expr);

#endif
