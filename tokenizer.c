#include "tokenizer.h"

#define IS_DIGIT(c) (('0' <= (c)) && ((c) <= '9'))
#define IS_IDENTIFIER_CHAR(c) ((('A' <= (c)) && ((c) <= 'Z')) || (('a' <= (c)) && ((c) <= 'z')) || (c) == '_')

char *tokenize(char *code, Token **tokens, ssize_t *len_ptr) {
    ssize_t len = 0;
    ssize_t line_number = 1;
    for (char *s = code; *s != '\0'; ++s) {
        Token tok;
        tok.line = line_number;
        #define ADD_TOK(t) *tokens = realloc(*tokens, (len + 1) * sizeof(Token)); tok.type = t; (*tokens)[(len++)] = tok;
        if (s[0] == '\n') {
            ++line_number;
            break;
        } else if (s[0] == '(') {
            ADD_TOK(TOK_LEFT_PAREN);
        } else if (s[0] == ')') {
            ADD_TOK(TOK_RIGHT_PAREN);
        } else if (s[0] == '{') {
            ADD_TOK(TOK_LEFT_BRACE);
        } else if (s[0] == '}') {
            ADD_TOK(TOK_RIGHT_BRACE);
        } else if (s[0] == ',') {
            ADD_TOK(TOK_COMMA);
        } else if (s[0] == ';') {
            ADD_TOK(TOK_SEMICOLON);
        } else if (s[0] == '.') {
            ADD_TOK(TOK_DOT);
        } else if (s[0] == '-' && s[1] == '>') {
            ++s;
            ADD_TOK(TOK_ARROW);
        } else if (s[0] == '/') {
            ADD_TOK(TOK_SLASH);
        } else if (s[0] == '*') {
            ADD_TOK(TOK_STAR);
        } else if (s[0] == '+') {
            if (s[1] == '+') {
                ++s;
                ADD_TOK(TOK_INC);
            } else {
                ADD_TOK(TOK_PLUS);
            }
        } else if (s[0] == '-') {
            if (s[1] == '-') {
                ++s;
                ADD_TOK(TOK_DEC);
            } else {
                ADD_TOK(TOK_MINUS);
            }
        } else if (s[0] == '&') {
            // doesn't find ands by design - this is the parser's job, since both &&x and x&&y are valid.
            ADD_TOK(TOK_AMPERSAND);
        } else if (s[0] == '|') {
            if (s[1] == '|') {
                ++s;
                ADD_TOK(TOK_OR);
            } else {
                ADD_TOK(TOK_BITWISE_OR);
            }
        } else if (s[0] == '^') {
            ADD_TOK(TOK_BITWISE_XOR);
        } else if (s[0] == '!') {
            ADD_TOK(TOK_NOT);
        } else if (s[0] == '~') {
            ADD_TOK(TOK_BITWISE_NOT);
        } else if (s[0] == '=') {
            if (s[1] == '=') {
                ++s;
                ADD_TOK(TOK_EQUAL_EQUAL);
            } else {
                ADD_TOK(TOK_EQUAL);
            }
        } else if (s[0] == '>') {
            if (s[1] == '=') {
                ++s;
                ADD_TOK(TOK_GREATER_EQUAL);
            } else {
                ADD_TOK(TOK_GREATER);
            }
        } else if (s[0] == '<') {
            if (s[1] == '=') {
                ++s;
                ADD_TOK(TOK_LESS_EQUAL);
            } else {
                ADD_TOK(TOK_LESS);
            }
        } else if (s[0] == '\'') {
            if (s[1] == '\0' || s[2] != '\'') {
                char *str = malloc(56);
                sprintf(str, "SyntaxError: started char on line %zu", line_number);
                return str;
            }
            *(tok.content) = s[1];
            ADD_TOK(TOK_CHAR);
        } else if (s[0] == '\"') {
            ssize_t result_len = 1;
            char *result = malloc(result_len);
            char *c = s;
            while (*c != '\0') {
                result = realloc(result, result_len++);
                if (*c == '\\') {
                    ++c;
                    if (*c == 'n') {
                        result[result_len - 1] = '\n';
                    } else if (*c == 'r') {
                        result[result_len - 1] = '\r';
                    } else if (*c == 't') {
                        result[result_len - 1] = '\t';
                    } else if (*c == '"') {
                        result[result_len - 1] = '"';
                    } else {
                        free(result);
                        char *str = malloc(62);
                        sprintf(str, "SyntaxError: unknown escape code on line %zu", line_number);
                        return str;
                    }
                } else if (*c == '"') {
                    break;
                } else {
                    result[result_len - 1] = *c;
                }
                ++c;
            }
            result[result_len] = '\0';
            tok.content = result;
            ADD_TOK(TOK_STRING);
        }
        #undef ADD_TOK
        // missing:
        // TOK_IF, TOK_ELSE, TOK_FOR, TOK_WHILE, TOK_RETURN,
        // TOK_IDENTIFIER, TOK_NUMBER,
        // TOK_TYPEDEF, TOK_STATIC, TOK_CONST, TOK_ENUM, TOK_STRUCT, TOK_PREPROCESSOR_DIRECTIVE
    }
    if (len_ptr != NULL) {
        *len_ptr = len;
    }
    return "";
}

#undef IS_DIGIT
#undef IS_IDENTIFIER_CHAR
