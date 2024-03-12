#ifndef BOBO_LANG_COMPILER_LEXER_H
#define BOBO_LANG_COMPILER_LEXER_H

#include <stdio.h>

#include "meta.h"
#include "token.h"

typedef struct lexer Lexer;

Lexer *init_lexer(Meta *meta);
token *scan(Lexer *lexer);
void free_lexer(Lexer *lexer);



#endif