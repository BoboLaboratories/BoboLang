#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

#include "token.h"
#include "lib/console.h"

static int peek = ' ';
static int line = 1;

static token reset(token tok) {
    peek = ' ';
    return tok;
}

token scan(FILE *fptr) {
    while (peek == ' ' || peek == '\t' || peek == '\n' || peek == '\r') {
        if (peek == '\n') {
            line++;
        }
        peek = fgetc(fptr);
    }

    switch (peek) {
    case '(':
        return reset(TOK_LPT);
    case ')':
        return reset(TOK_RPT);
    case '{':
        return reset(TOK_LPG);
    case '}':
        return reset(TOK_RPG);
    case '.':
        return reset(TOK_DOT);
    case '*':
        return reset(TOK_MUL);
    case '+':
        return reset(TOK_PLUS);
    case '/':
        return reset(TOK_DIV);
    case ',':
        return reset(TOK_COMMA);
    case '<':
        return ((peek = fgetc(fptr)) == '=') ? reset(TOK_LE) : TOK_LT;
    case '>':
        return ((peek = fgetc(fptr)) == '=') ? reset(TOK_GE) : TOK_GT;
    case '!':
        return ((peek = fgetc(fptr)) == '=') ? reset(TOK_NEQ) : TOK_NOT;
    case '=':
        return ((peek = fgetc(fptr)) == '=') ? reset(TOK_EQ) : TOK_ASSIGN;
    case '-':
        return ((peek = fgetc(fptr)) == '>') ? reset(TOK_ARROW) : TOK_MINUS;
    case '&':
        if ((peek = fgetc(fptr)) != '&') {
            print(E, "illegal symbol &%c at line %d\n", peek, line);
            return TOK_ERR;
        }
        return reset(TOK_AND);
    case '|':
        if ((peek = fgetc(fptr)) != '|') {
            print(E, "illegal symbol |%c at line %d\n", peek, line);
            return TOK_ERR;
        }
        return reset(TOK_OR);
    case EOF:
        return TOK_EOF;
    default:
        if (isalpha(peek) || peek == '_') {
            int underscore_only = peek == '_';
            long start = ftell(fptr) - 1;
            do {
                underscore_only &= peek == '_';
                peek = fgetc(fptr);
            } while (isalnum(peek) || peek == '_');

            long end = ftell(fptr);
            if (peek != EOF) {
                end--;
            }
            long len = end - start;
            char *lexeme = malloc(len + 1);

            fseek(fptr, start, SEEK_SET);
            fread(lexeme, len, 1, fptr);
            *(lexeme + len) = '\0';

            if (underscore_only) {
                print(E, "illegal symbol %s at line %d\n", lexeme, line);
                return TOK_ERR;
            } else {
                token ret;
                if (strcmp(lexeme, "import") == 0) {
                    ret = TOK_IMPORT;
                } else if (strcmp(lexeme, "module") == 0) {
                    ret = TOK_MODULE;
                } else if (strcmp(lexeme, "fun") == 0) {
                    ret = TOK_FUN;
                } else if (strcmp(lexeme, "private") == 0) {
                    ret = TOK_PRIVATE;
                } else if (strcmp(lexeme, "native") == 0) {
                    ret = TOK_NATIVE;
                } else if (strcmp(lexeme, "const") == 0) {
                    ret = TOK_CONST;
                }  else if (strcmp(lexeme, "check") == 0) {
                    ret = TOK_CHECK;
                } else {
                    ret = (token) {ID, lexeme};
                }

                if (ret.tag != ID) {
                    free(lexeme);
                }

                return reset(ret);
            }
        }
        return TOK_EOF;
    }
}