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
            ADD_TOK(TOK_PLUS);
        } else if (s[0] == '-') {
            ADD_TOK(TOK_MINUS);
        } else if (s[0] == '&') {
            // doesn't find ands by design - this is the parser's job, since both &&x and x&&y are valid.
            ADD_TOK(TOK_AMPERSAND);
        } else if (s[0] == '|') {
            if (s[1] == '|') {
                ++s;
                ADD_TOK(TOK_OR);
            } else {
                ADD_TOK(TOK_BITOR);
            }
        } else if (s[0] == '^') {
            ADD_TOK(TOK_BITXOR);
        } else if (s[0] == '!') {
            if (s[1] == '=') {
                ++s;
                ADD_TOK(TOK_NEQ);
            } else {
                ADD_TOK(TOK_NOT);
            }
        } else if (s[0] == '~') {
            ADD_TOK(TOK_BITNOT);
        } else if (s[0] == '=') {
            if (s[1] == '=') {
                ++s;
                ADD_TOK(TOK_EQ);
            } else {
                ADD_TOK(TOK_ASSIGN);
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
            s += i - 1;
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
            } else {
                ADD_TOK(TOK_IDENTIFIER);
            }
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

char *token_to_string(Token t) {
    #define return_str(s) char *result = malloc(strlen(s) + 1); strcpy(result, s); return result;
    switch (t.type) {
        case TOK_LEFT_PAREN: {
            return_str("(");
        }
        case TOK_RIGHT_PAREN: {
            return_str(")");
        }
        case TOK_LEFT_BRACE: {
            return_str("{");
        }
        case TOK_RIGHT_BRACE: {
            return_str("}");
        }
        case TOK_IF: {
            return_str("if");
        }
        case TOK_ELSE: {
            return_str("else");
        }
        case TOK_FOR: {
            return_str("for");
        }
        case TOK_WHILE: {
            return_str("while");
        }
        case TOK_RETURN: {
            return_str("return");
        }
        case TOK_COMMA: {
            return_str(",");
        }
        case TOK_SEMICOLON: {
            return_str(";");
        }
        case TOK_DOT: {
            return_str(".");
        }
        case TOK_ARROW: {
            return_str("->");
        }
        case TOK_PLUS: {
            return_str("+");
        }
        case TOK_MINUS: {
            return_str("-");
        }
        case TOK_SLASH: {
            return_str("/");
        }
        case TOK_STAR: {
            return_str("*");
        }
        case TOK_AMPERSAND: {
            return_str("&");
        }
        case TOK_OR: {
            return_str("||");
        }
        case TOK_BITOR: {
            return_str("|");
        }
        case TOK_BITXOR: {
            return_str("^");
        }
        case TOK_NOT: {
            return_str("!");
        }
        case TOK_BITNOT: {
            return_str("~");
        }
        case TOK_ASSIGN: {
            return_str("=");
        }
        case TOK_EQ: {
            return_str("==");
        }
        case TOK_NEQ: {
            return_str("!=")
        }
        case TOK_GREATER: {
            return_str(">");
        }
        case TOK_GREATER_EQUAL: {
            return_str(">=");
        }
        case TOK_LESS: {
            return_str("<");
        }
        case TOK_LESS_EQUAL: {
            return_str("<=");
        }
        case TOK_IDENTIFIER: {
            char *result = malloc(13 + strlen(t.content));
            strcpy(result, "(identifier)");
            strcpy(result + 12, t.content);
            return result;
        }
        case TOK_STRING: {
            char *result = malloc(3 + strlen(t.content));
            result[0] = '"';
            memcpy(result + 1, t.content, strlen(t.content));
            result[strlen(t.content) + 1] = '"';
            result[strlen(t.content) + 2] = '\0';
            return result;
        }
        case TOK_NUMBER: {
            char *result = malloc(strlen(t.content) + 1);
            strcpy(result, t.content);
            return result;
        }
        case TOK_TYPEDEF: {
            return_str("typedef");
        }
        case TOK_STATIC: {
            return_str("static");
        }
        case TOK_CONST: {
            return_str("const");
        }
        case TOK_ENUM: {
            return_str("enum");
        }
        case TOK_STRUCT: {
            return_str("struct");
        }
        case TOK_PREPROCESSOR_DIRECTIVE: {
            return_str("(preprocessor directive)");
        }
        default: {
            return_str("(unknown)");
        }
    }
}

#undef IS_DIGIT
#undef IS_IDENTIFIER_CHAR
