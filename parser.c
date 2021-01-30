#include "parser.h"

void Expr_free(Expr expr) {
    switch (expr.type) {
        // unaries
        case EXPR_POST_INC:
        case EXPR_POST_DEC:
        case EXPR_PRE_INC:
        case EXPR_PRE_DEC:
        case EXPR_UNARY_PLUS:
        case EXPR_UNARY_MINUS:
        case EXPR_NOT:
        case EXPR_BITNOT:
        case EXPR_DEREF:
        case EXPR_ADDR:
        case EXPR_SIZEOF:
            free(expr.data);
            break;
        case EXPR_FUNC_CALL:
        case EXPR_ARRAY_INDEX:
        case EXPR_MUL:
        case EXPR_DIV:
        case EXPR_MOD:
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_SHLEFT:
        case EXPR_SHRIGHT:
        case EXPR_GREATER:
        case EXPR_GREATER_EQUAL:
        case EXPR_LESS:
        case EXPR_LESS_EQUAL:
        case EXPR_EQ:
        case EXPR_NEQ:
        case EXPR_AND:
        case EXPR_OR:
        case EXPR_BITAND:
        case EXPR_BITXOR:
        case EXPR_BITOR:
        case EXPR_TERNARY:
        {
            ExprTwo data = *(ExprTwo *)(expr.data);
            Expr_free(data.a);
            Expr_free(data.b);
            free(expr.data);
            break;
        }
        // missing: EXPR_DOT, EXPR_ARROW, EXPR_ASSIGN, EXPR_ASSIGN_*
        // idk how they'll be stored
    }
}
