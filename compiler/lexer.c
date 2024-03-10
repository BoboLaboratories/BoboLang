#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

#include "lexer/token.h"
#include "lexer/lexer.h"
#include "lib/console.h"

struct lexer {
    FILE *fptr;
    char peek;
    int line;
};


/* reads the next char from file */
#define next()                  (l->peek = fgetc(l->fptr))

/* make token with well known lexeme */
#define mktok(tok)              make_token(l, tok, false)

/* make token with well known lexeme and reset peek */
#define mktokr(tok)             make_token(l, tok, true)

/* make token with the given tag and lexeme */
#define mktokl(tag, lexeme)     make_token(l, tag, lexeme, false)

/* make token with the given tag and lexeme and reset peek */
#define mktoklr(tag, lexeme)    make_token(l, tag, lexeme, true)


static token *make_token(Lexer *l, int tag, char *lexeme, bool reset);
static void single_line_comment(Lexer *l);
static void multi_line_comment(Lexer *l);
static int is_whitespace(Lexer *l);


Lexer *init_lexer(FILE *fptr) {
    Lexer *l = malloc(sizeof(Lexer));

    l->fptr = fptr;
    l->peek = ' ';
    l->line = 1;

    return l;
}

void free_lexer(Lexer *l) {
    fclose(l->fptr);
    free(l);
}

token *scan(Lexer *l) {
    while (is_whitespace(l)) {
        if (l->peek == '\n') {
            l->line++;
        }
        next();
    }

    switch (l->peek) {
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
        switch (next()) {
        case '/':
            single_line_comment(l);
            return scan(l);
        case '*':
            multi_line_comment(l);
            return scan(l);
        default:
            return mktok(TOK_DIV);
        }
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
            print(E, "illegal symbol &%c at line %d\n", l->peek, l->line);
            exit(EXIT_FAILURE);
        }
        return mktokr(TOK_AND);
    case '|':
        if (next() != '|') {
            print(E, "illegal symbol |%c at line %d\n", l->peek, l->line);
            exit(EXIT_FAILURE);
        }
        return mktokr(TOK_OR);
    case EOF:
        return mktokl(EOF, NULL);
    default:
        if (isalpha(l->peek) || l->peek == '_') {
            int underscore_only = l->peek == '_';
            long start = ftell(l->fptr) - 1;
            do {
                underscore_only &= l->peek == '_';
                next();
            } while (isalnum(l->peek) || l->peek == '_');

            long end = ftell(l->fptr);
            if (l->peek != EOF) {
                end--;
            }
            long len = end - start;
            char *lexeme = malloc(len + 1);

            fseek(l->fptr, start, SEEK_SET);
            fread(lexeme, len, 1, l->fptr);
            *(lexeme + len) = '\0';

            if (underscore_only) {
                print(E, "illegal symbol %s at line %d\n", lexeme, l->line);
                exit(EXIT_FAILURE);
            } else {
                token *ret;
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
                    ret = mktoklr(ID, lexeme);
                }

                if (ret->tag != ID) {
                    free(lexeme);
                }

                return ret;
            }
        }

        print(E, "erroneous symbol %c at line %d\n", l->peek, l->line);
        exit(EXIT_FAILURE);
    }
}

static void multi_line_comment(Lexer *l) {
    int line = l->line;
    int state = 0;

    do {
        next();
        if (state == 0) {
            if (l->peek == '*') {
                state = 1;
            }
        } else {
            if (l->peek == '/') {
                state = 2;
            } else if (l->peek != '*') {
                state = 0;
            }
        }
    } while (state != 2 && l->peek != EOF);

    if (state == 2) {
        l->peek = ' ';
    } else {
        print(E, "unclosed multi line comment starting at line %d\n", line);
        exit(EXIT_FAILURE);
    }
}

static void single_line_comment(Lexer *l) {
    do {
        next();
    } while (l->peek != '\n' && l->peek != EOF);
}

static int is_whitespace(Lexer *l) {
    return l->peek == ' '
           || l->peek == '\t'
           || l->peek == '\n'
           || l->peek == '\r';
}

static token *make_token(Lexer *l, int tag, char *lexeme, bool reset) {
    if (reset) {
        l->peek = ' ';
    }

    token *tok = malloc(sizeof(token));
    tok->line = l->line;
    tok->lexeme = lexeme;
    tok->tag = tag;

    return tok;
}