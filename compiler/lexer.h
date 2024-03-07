#ifndef BOBO_LANG_COMPILER_LEXER_H
#define BOBO_LANG_COMPILER_LEXER_H

#include <stdio.h>

#include "token.h"

#define token_lexeme_needs_freeing(tok) (tok->tag == ID)

typedef struct {
    FILE *fptr;
    char peek;
    int line;
} lexer;

lexer *init_lexer(FILE *fptr);
token scan(lexer *lexer);

#endif