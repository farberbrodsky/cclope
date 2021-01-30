#include "tokenizer.h"

#define IS_DIGIT(c) (('0' <= (c)) && ((c) <= '9'))
#define IS_IDENTIFIER_CHAR(c) ((('A' <= (c)) && ((c) <= 'Z')) || (('a' <= (c)) && ((c) <= 'z')) || (c) == '_')

char *tokenize(char *code, Token **tokens, ssize_t *len_ptr) {
    ssize_t len = 0;
    ssize_t line_number = 1;
    *tokens = malloc(0);
    for (char *s = code; *s != '\0'; ++s) {
        Token tok;
        tok.line = line_number;
        tok.content = NULL;
        #define ADD_TOK(t) *tokens = realloc(*tokens, (len + 1) * sizeof(Token)); tok.type = t; (*tokens)[(len++)] = tok;
        if (s[0] == '\n') {
            ++line_number;
        } else if (s[0] == ' ' || s[0] == '\t') {
            // do nothing
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
            while (*s != '\0') {
                result = realloc(result, result_len++);
                if (*s == '\\') {
                    ++s;
                    if (*s == 'n') {
                        result[result_len - 1] = '\n';
                    } else if (*s == 'r') {
                        result[result_len - 1] = '\r';
                    } else if (*s == 't') {
                        result[result_len - 1] = '\t';
                    } else if (*s == '"') {
                        result[result_len - 1] = '"';
                    } else {
                        free(result);
                        char *str = malloc(62);
                        sprintf(str, "SyntaxError: unknown escape code on line %zu", line_number);
                        return str;
                    }
                } else if (*s == '"') {
                    break;
                } else {
                    result[result_len - 1] = *s;
                }
                ++s;
            }
            result[result_len] = '\0';
            tok.content = result;
            ADD_TOK(TOK_STRING);
        } else if (IS_DIGIT(s[0])) {
            // number - keep in a string every digit or .
            ssize_t i = 0;
            for (; IS_DIGIT(s[i]) || s[i] == '.'; ++i);
            tok.content = malloc(i + 1);
            memcpy(tok.content, s, i * sizeof(char));
            tok.content[i] = '\0';
            ADD_TOK(TOK_NUMBER);
            s += i;
        } else if (IS_IDENTIFIER_CHAR(s[0])) {
            // read identifier to the end
            ssize_t i = 0;
            for (; IS_IDENTIFIER_CHAR(s[i]) || IS_DIGIT(s[i]); ++i);
            tok.content = malloc(i + 1);
            memcpy(tok.content, s, i * sizeof(char));
            tok.content[i] = '\0';
            if (i == 2 && strcmp(tok.content, "if") == 0) {
                free(tok.content);
                tok.content = NULL;
                ADD_TOK(TOK_IF);
            } else if (i == 3 && strcmp(tok.content, "for") == 0) {
                free(tok.content);
                tok.content = NULL;
                ADD_TOK(TOK_FOR);
            } else if (i == 4 && strcmp(tok.content, "else") == 0) {
                free(tok.content);
                tok.content = NULL;
                ADD_TOK(TOK_ELSE);
            } else if (i == 4 && strcmp(tok.content, "enum") == 0) {
                free(tok.content);
                tok.content = NULL;
                ADD_TOK(TOK_ELSE);
            } else if (i == 5 && strcmp(tok.content, "while") == 0) {
                free(tok.content);
                tok.content = NULL;
                ADD_TOK(TOK_WHILE);
            } else if (i == 5 && strcmp(tok.content, "const") == 0) {
                free(tok.content);
                tok.content = NULL;
                ADD_TOK(TOK_CONST);
            } else if (i == 6 && strcmp(tok.content, "return") == 0) {
                free(tok.content);
                tok.content = NULL;
                ADD_TOK(TOK_RETURN);
            } else if (i == 6 && strcmp(tok.content, "static")) {
                free(tok.content);
                tok.content = NULL;
                ADD_TOK(TOK_TYPEDEF);
            } else if (i == 6 && strcmp(tok.content, "struct")) {
                free(tok.content);
                tok.content = NULL;
                ADD_TOK(TOK_STRUCT);
            } else if (i == 7 && strcmp(tok.content, "typedef") == 0) {
                free(tok.content);
                tok.content = NULL;
                ADD_TOK(TOK_TYPEDEF);
            }
            ADD_TOK(TOK_IDENTIFIER);
            s += i;
        } else if (*s == '#') {
            char *str = malloc(62);
            sprintf(str, "SyntaxError: cclope currently does not support preprocessor directive on line %zu", line_number);
            return str;
        } else {
            char *str = malloc(62);
            sprintf(str, "SyntaxError: unknown token on line %zu", line_number);
            return str;
        }
        #undef ADD_TOK
        // missing:
        // TOK_PREPROCESSOR_DIRECTIVE
    }
    if (len_ptr != NULL) {
        *len_ptr = len;
    }
    return "";
}

#undef IS_DIGIT
#undef IS_IDENTIFIER_CHAR
