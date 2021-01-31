#include "parser.h"

char *parse_expr(Token **tokens, Expr *expr, ssize_t tok_count) {
    // find a place that has to be after the expression, which is ; or EOF
    if (tok_count == 0) {
        Token *tok_end = *tokens;
        while (tok_end->type != TOK_SEMICOLON && tok_end->type != TOK_EOF) {
            ++tok_end;
            ++tok_count;
        }
    }
    Expr *expressions = malloc(tok_count * sizeof(Expr));
    ssize_t expressions_len = 0;
    // make an unkown expression for each
    for (ssize_t i = 0; i < tok_count; ++i) {
        if ((*tokens)[i].type == TOK_IDENTIFIER) {
            expressions[i].type = EXPR_IDENTIFIER;
            expressions[i].data = malloc(strlen((*tokens)[i].content) + 1);
            strcpy(expressions[i].data, (*tokens)[i].content);
        } else if ((*tokens)[i].type == TOK_NUMBER) {
            expressions[i].type = EXPR_NUMBER;
            expressions[i].data = malloc(strlen((*tokens)[i].content) + 1);
            strcpy(expressions[i].data, (*tokens)[i].content);
        } else {
            expressions[i].type = EXPR_UNKOWN;
            expressions[i].data = &(*tokens)[i];
        }
    }
    #define MOVE_BACK(x, d) memmove(expressions + x, expressions + x - d, sizeof(Expr) * (tok_count - x)); expressions_len -= d;
    // precidence 0: recursively go into parentheses
    ssize_t paren_level = 0;
    ssize_t paren_start = 0;
    for (ssize_t i = 0; i < tok_count; ++i) {
        Expr e = expressions[i];
        if (e.type == EXPR_UNKOWN && ((Token *)(e.data))->type == TOK_LEFT_PAREN) {
            if (paren_level++ == 0) {
                paren_start = i;
            }
        } else if (e.type == EXPR_UNKOWN && ((Token *)(e.data))->type == TOK_RIGHT_PAREN) {
            if (--paren_level == 0) {
                // TODO: find function calls, which are when there's an identifier right before the parentheses
                // parse everything in parentheses
                ssize_t paren_len = i - paren_start - 1;
                if (paren_len != 0) {
                    Expr in_paren;
                    char *err = parse_expr(tokens, &in_paren, paren_len);
                    if (err) {
                        free(expressions);
                        return err;
                    }
                    expressions[paren_start] = in_paren;
                    MOVE_BACK(i, paren_len);
                }
            }
        }
    }
    // precidence 1: EXPR_POST_INC, EXPR_POST_DEC
    for (ssize_t i = 0; i < expressions_len; ++i) {
        Expr *e = &expressions[i];
        if (e[1].type == EXPR_UNKOWN && e[2].type == EXPR_UNKOWN) {
            // look for x++ or x--
            Token t1 = *((Token *)e[1].data);
            Token t2 = *((Token *)e[2].data);
            if (t1.type == TOK_PLUS && t2.type == TOK_PLUS) {
                // post increment
                Expr inc_expr;
                inc_expr.type = EXPR_POST_INC;
                inc_expr.data = &expressions[i];
                expressions[i] = inc_expr;
                MOVE_BACK(i + 3, 2);
            } else if (t1.type == TOK_MINUS && t2.type == TOK_MINUS) {
                // post decrease
                Expr dec_expr;
                dec_expr.type = EXPR_POST_DEC;
                dec_expr.data = &expressions[i];
                expressions[i] = dec_expr;
                MOVE_BACK(i + 3, 2);
            }
        }
    }
    free(expressions);
    return "";
    #undef MOVE_BACK
}

void Expr_free(Expr expr) {
    switch (expr.type) {
        // unaries
        case EXPR_IDENTIFIER:
        case EXPR_NUMBER:
            free(expr.data);
            break;
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
            Expr_free(*((Expr *)expr.data));
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
        // EXPR_UNKOWN should not be freed
        // idk how they'll be stored
    }
}
