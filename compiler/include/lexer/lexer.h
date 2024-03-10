#ifndef BOBO_LANG_COMPILER_LEXER_H
#define BOBO_LANG_COMPILER_LEXER_H

#include <stdio.h>

#include "token.h"

typedef struct lexer Lexer;

Lexer *init_lexer(FILE *fptr);
token *scan(Lexer *l);
void free_lexer(Lexer *l);

#endif