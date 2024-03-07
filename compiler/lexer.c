#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

#include "token.h"
#include "lexer.h"
#include "lib/console.h"

static int is_whitespace(lexer *lexer);
static token make_token(lexer *lexer, int tag, char *lexeme, int reset);

#define next()      (lexer->peek = fgetc(lexer->fptr))
#define mktok(tok)  make_token(lexer, tok, 0)
#define mktokr(tok) make_token(lexer, tok, 1)

lexer *init_lexer(FILE *fptr) {
    lexer *lex = malloc(sizeof(lexer));
    lex->fptr = fptr;
    lex->peek = ' ';
    lex->line = 1;
    return lex;
}

token scan(lexer *lexer) {
    while (is_whitespace(lexer)) {
        if (lexer->peek == '\n') {
            lexer->line++;
        }
        next();
    }

    switch (lexer->peek) {
    case '(':
        return mktokr(TOK_LPT);
    case ')':
        return mktokr(TOK_RPT);
    case '{':
        return mktokr(TOK_LPG);
    case '}':
        return mktokr(TOK_RPG);
    case '.':
        return mktokr(TOK_DOT);
    case '*':
        return mktokr(TOK_MUL);
    case '+':
        return mktokr(TOK_PLUS);
    case '/':
        return mktokr(TOK_DIV);
    case ',':
        return mktokr(TOK_COMMA);
    case '<':
        return (next() == '=') ? mktokr(TOK_LE) : mktok(TOK_LT);
    case '>':
        return (next() == '=') ? mktokr(TOK_GE) : mktok(TOK_GT);
    case '!':
        return (next() == '=') ? mktokr(TOK_NEQ) : mktok(TOK_NOT);
    case '=':
        return (next() == '=') ? mktokr(TOK_EQ) : mktok(TOK_ASSIGN);
    case '-':
        return (next() == '>') ? mktokr(TOK_ARROW) : mktok(TOK_MINUS);
    case '&':
        if (next() != '&') {
            print(E, "illegal symbol &%c at line %d\n", lexer->peek, lexer->line);
            return mktok(TOK_ERR);
        }
        return mktokr(TOK_AND);
    case '|':
        if (next() != '|') {
            print(E, "illegal symbol |%c at line %d\n", lexer->peek, lexer->line);
            return mktok(TOK_ERR);
        }
        return mktokr(TOK_OR);
    case EOF:
        return mktok(TOK_EOF);
    default:
        if (isalpha(lexer->peek) || lexer->peek == '_') {
            int underscore_only = lexer->peek == '_';
            long start = ftell(lexer->fptr) - 1;
            do {
                underscore_only &= lexer->peek == '_';
                next();
            } while (isalnum(lexer->peek) || lexer->peek == '_');

            long end = ftell(lexer->fptr);
            if (lexer->peek != EOF) {
                end--;
            }
            long len = end - start;
            char *lexeme = malloc(len + 1);

            fseek(lexer->fptr, start, SEEK_SET);
            fread(lexeme, len, 1, lexer->fptr);
            *(lexeme + len) = '\0';

            if (underscore_only) {
                print(E, "illegal symbol %s at line %d\n", lexeme, lexer->line);
                return mktok(TOK_ERR);
            } else {
                token ret;
                if (strcmp(lexeme, "import") == 0) {
                    ret = mktokr(TOK_IMPORT);
                } else if (strcmp(lexeme, "fun") == 0) {
                    ret = mktokr(TOK_FUN);
                } else if (strcmp(lexeme, "private") == 0) {
                    ret = mktokr(TOK_PRIVATE);
                } else if (strcmp(lexeme, "native") == 0) {
                    ret = mktokr(TOK_NATIVE);
                } else if (strcmp(lexeme, "const") == 0) {
                    ret = mktokr(TOK_CONST);
                } else if (strcmp(lexeme, "var") == 0) {
                    ret = mktokr(TOK_VAR);
                } else if (strcmp(lexeme, "check") == 0) {
                    ret = mktokr(TOK_CHECK);
                } else {
                    ret = mktokr(TOK_ID);
                }

                if (ret.tag != ID) {
                    free(lexeme);
                }

                return ret;
            }
        }
        return mktok(TOK_ERR);
    }
}

static int is_whitespace(lexer *lexer) {
    return lexer->peek == ' '
           || lexer->peek == '\t'
           || lexer->peek == '\n'
           || lexer->peek == '\r';
}

static token make_token(lexer *lexer, int tag, char *lexeme, int reset) {
    if (reset) {
        lexer->peek = ' ';
    }
    return (token) {
            .tag = tag,
            .lexeme = lexeme,
            .line = lexer->line,
    };
}