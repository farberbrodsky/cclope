#ifndef GUARD_8a64339e_6312_11eb_b301_507b9dcc3ec8
#define GUARD_8a64339e_6312_11eb_b301_507b9dcc3ec8

#include "tokenizer.h"

typedef enum {
    EXPR_POST_INC='A', EXPR_POST_DEC='B', EXPR_FUNC_CALL='C',                      // precidence 1
    EXPR_ARRAY_INDEX='D', EXPR_DOT='E', EXPR_ARROW='F',                            // precidence 1
    EXPR_PRE_INC='G', EXPR_PRE_DEC='H', EXPR_UNARY_PLUS='I', EXPR_UNARY_MINUS='J', // precidence 2
    EXPR_NOT='K', EXPR_BITNOT='L', EXPR_DEREF='M', EXPR_ADDR='N', EXPR_SIZEOF='O', // precidence 2
    EXPR_MUL='P', EXPR_DIV='Q', EXPR_MOD='R',                                      // precidence 3
    EXPR_ADD='S', EXPR_SUB='T',                                                    // precidence 4
    EXPR_SHLEFT='U', EXPR_SHRIGHT='V',                                             // precidence 5
    EXPR_GREATER='W', EXPR_GREATER_EQUAL='X', EXPR_LESS='Y', EXPR_LESS_EQUAL='Z',  // precidence 6
    EXPR_EQ='a', EXPR_NEQ='b',                                                     // precidence 7
    EXPR_BITAND='c',                                                               // precidence 8
    EXPR_BITXOR='d',                                                               // precidence 9
    EXPR_BITOR='e',                                                                // precidence 10
    EXPR_AND='f',                                                                  // precidence 11
    EXPR_OR='g',                                                                   // precidence 12
    EXPR_TERNARY='h',                                                              // precidence 13
    EXPR_ASSIGN='j',        EXPR_ASSIGN_ADD='k',     EXPR_ASSIGN_SUB='l',          // precidence 14
    EXPR_ASSIGN_MUL='m',    EXPR_ASSIGN_DIV='n',     EXPR_ASSIGN_MOD='o',          // precidence 14
    EXPR_ASSIGN_SHLEFT='p', EXPR_ASSIGN_SHRIGHT='q',                               // precidence 14
    EXPR_ASSIGN_BITAND='r', EXPR_ASSIGN_BITXOR='s',  EXPR_ASSIGN_BITOR='t',        // precidence 14
    EXPR_COMMA='u',                                                                // precidence 15
    EXPR_UNKOWN='v',                                                               // intermediary for unparsed tokens
    EXPR_IDENTIFIER='w', EXPR_NUMBER='x'                                           // same as with the token
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

// tok_count is how many tokens are there after this token.
// Gets end where it should stop looking for tokens or 0 to look automatically.
// Returns "" if it parsed successfuly or a detailed explanation of the error if it failed.
char *parse_expr(Token *tokens, Expr *expr, ssize_t tok_count);
void Expr_free(Expr expr);
char *Expr_to_string(Expr e);

#endif
