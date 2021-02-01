#include "parser.h"

char *parse_expr(Token *tokens, Expr *expr, ssize_t tok_count) {
    // find a place that has to be after the expression, which is ; or EOF
    if (tok_count == 0) {
        Token *tok_end = tokens;
        while (tok_end->type != TOK_SEMICOLON && tok_end->type != TOK_EOF) {
            ++tok_end;
            ++tok_count;
        }
    }
    Expr *expressions = malloc(tok_count * sizeof(Expr));
    ssize_t expressions_len = tok_count;
    // make an expression for each token
    for (ssize_t i = 0; i < tok_count; ++i) {
        if (tokens[i].type == TOK_IDENTIFIER) {
            expressions[i].type = EXPR_IDENTIFIER;
            expressions[i].data = malloc(strlen(tokens[i].content) + 1);
            strcpy(expressions[i].data, tokens[i].content);
        } else if (tokens[i].type == TOK_NUMBER) {
            expressions[i].type = EXPR_NUMBER;
            expressions[i].data = malloc(strlen(tokens[i].content) + 1);
            strcpy(expressions[i].data, tokens[i].content);
        } else {
            expressions[i].type = EXPR_UNKOWN;
            expressions[i].data = &tokens[i];
        }
    }
    #define MOVE_BACK(x, d) memmove(expressions + (x) - (d), expressions + (x), sizeof(Expr) * (expressions_len - (x))); expressions_len -= (d);
    // precidence 0: recursively go into parentheses
    ssize_t paren_level = 0;
    ssize_t paren_start = 0;
    for (ssize_t i = 0; i < expressions_len; ++i) {
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
                    char *err = parse_expr(tokens + paren_start, &in_paren, paren_len);
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
        if ((i + 2) < expressions_len && e[1].type == EXPR_UNKOWN && e[2].type == EXPR_UNKOWN) {
            // look for x++ or x--
            Token t1 = *((Token *)e[1].data);
            Token t2 = *((Token *)e[2].data);
            if (t1.type == TOK_PLUS && t2.type == TOK_PLUS) {
                // post increment
                printf("postinc %zu\n", expressions_len);
                Expr inc_expr;
                inc_expr.type = EXPR_POST_INC;
                Expr *expressions_i_copy = malloc(sizeof(Expr));
                memcpy(expressions_i_copy, &expressions[i], sizeof(Expr));
                inc_expr.data = expressions_i_copy;
                expressions[i] = inc_expr;
                MOVE_BACK(i + 3, 2);
                --i; // to allow for x++++, don't increase i
            } else if (t1.type == TOK_MINUS && t2.type == TOK_MINUS) {
                // post decrease
                Expr dec_expr;
                dec_expr.type = EXPR_POST_DEC;
                Expr *expressions_i_copy = malloc(sizeof(Expr));
                memcpy(expressions_i_copy, &expressions[i], sizeof(Expr));
                dec_expr.data = expressions_i_copy;
                expressions[i] = dec_expr;
                MOVE_BACK(i + 3, 2);
                --i; // to allow for x---- don't increase i
            }
        }
    }
    *expr = expressions[0];
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

char *Expr_to_string(Expr e) {
    char *s, *c;
    ssize_t s_len;
    switch (e.type) {
        case EXPR_IDENTIFIER:
            c = malloc(strlen(e.data) + 1);
            strcpy(c, e.data);
            return c;
        case EXPR_POST_INC:
        case EXPR_POST_DEC:
            s = Expr_to_string(*((Expr *)e.data));
            s_len = strlen(s);
            c = malloc(s_len + 5);
            strcpy(c + 1, s);
            free(s);
            c[0] = '(';
            c[s_len + 1] = ')';
            break;
        case EXPR_FUNC_CALL:
            s = Expr_to_string(*((Expr *)e.data));
            s_len = strlen(s);
            c = malloc(s_len + 3);
            strcpy(c, s);
            free(s);
            c[s_len] = '(';
            c[s_len + 1] = ')';
            c[s_len + 2] = '\0';
            return c;
        default:
            c = malloc(10 * sizeof(char));
            strcpy(c, "(unknown)");
            return c;
    }
    switch (e.type) {
        case EXPR_POST_INC:
            strcpy(c + s_len + 2, "++");
            return c;
        case EXPR_POST_DEC:
            strcpy(c + s_len + 2, "--");
            return c;
    }
}
