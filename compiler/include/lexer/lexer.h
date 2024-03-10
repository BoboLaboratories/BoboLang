#ifndef BOBO_LANG_COMPILER_LEXER_H
#define BOBO_LANG_COMPILER_LEXER_H

#include <stdio.h>

#include "token.h"

typedef struct lexer lexer;

lexer *init_lexer(FILE *fptr);
token *scan(lexer *l);
void free_lexer(lexer *l);

#endif