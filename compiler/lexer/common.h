#ifndef BOBO_LANG_COMPILER_LEXER_COMMON_H
#define BOBO_LANG_COMPILER_LEXER_COMMON_H

#include <stdbool.h>

#include "meta.h"
#include "lexer/token.h"
#include "lexer/lexer.h"

struct lexer {
    Meta *meta;
    FILE *fptr;
    char peek;
    long buf_start;
    long line_start;
    unsigned int line;
    unsigned int column;
};

/* make token with well known lexeme */
#define mktok(tok)              make_token(lexer, tok, false)

/* make token with well known lexeme and reset peek */
#define mktokr(tok)             make_token(lexer, tok, true)

/* make token with the given tag and lexeme */
#define mktokl(tag, lexeme)     make_token(lexer, tag, lexeme, false)

/* make token with the given tag and lexeme and reset peek */
#define mktoklr(tag, lexeme)    make_token(lexer, tag, lexeme, true)

char next(Lexer * lexer);

token *make_token(Lexer *lexer, int tag, char *lexeme, bool reset);

void start_buffering(Lexer *lexer);

char *get_buffer(Lexer *lexer);

int is_whitespace(char c);

void lexer_err(Lexer *lexer, char *msg);

#endif