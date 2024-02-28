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
        return reset(LPT);
    case ')':
        return reset(RPT);
    case '{':
        return reset(LPG);
    case '}':
        return reset(RPG);
    case '.':
        return reset(DOT);
    case '*':
        return reset(MUL);
    case '+':
        return reset(PLUS);
    case '/':
        return reset(DIV);
    case ',':
        return reset(COMMA);
    case '<':
        return ((peek = fgetc(fptr)) == '=') ? reset(LE) : LT;
    case '>':
        return ((peek = fgetc(fptr)) == '=') ? reset(GE) : GT;
    case '!':
        return ((peek = fgetc(fptr)) == '=') ? reset(NEQ) : NOT;
    case '=':
        return ((peek = fgetc(fptr)) == '=') ? reset(EQ) : ASSIGN;
    case '-':
        return ((peek = fgetc(fptr)) == '>') ? reset(ARROW) : MINUS;
    case '&':
        if ((peek = fgetc(fptr)) != '&') {
            print(E, "illegal symbol &%c at line %d\n", peek, line);
            return TOK_ERR;
        }
        return reset(AND);
    case '|':
        if ((peek = fgetc(fptr)) != '|') {
            print(E, "illegal symbol |%c at line %d\n", peek, line);
            return TOK_ERR;
        }
        return reset(OR);
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
                    ret = IMPORT;
                } else if (strcmp(lexeme, "module") == 0) {
                    ret = MODULE;
                } else if (strcmp(lexeme, "fun") == 0) {
                    ret = FUN;
                } else if (strcmp(lexeme, "private") == 0) {
                    ret = PRIVATE;
                } else if (strcmp(lexeme, "public") == 0) {
                    ret = PUBLIC;
                } else if (strcmp(lexeme, "const") == 0) {
                    ret = CONST;
                } else if (strcmp(lexeme, "var") == 0) {
                    ret = VAR;
                } else if (strcmp(lexeme, "check") == 0) {
                    ret = CHECK;
                } else {
                    ret = (token) {ID, lexeme};
                }

                if (ret.tag != ID) {
                    free(lexeme);
                }

                return ret;
            }
        }
        return TOK_EOF;
    }
}