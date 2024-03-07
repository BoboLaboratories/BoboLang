#ifndef BOBO_LANG_COMPILER_TRANSLATOR_H
#define BOBO_LANG_COMPILER_TRANSLATOR_H

#include <stdio.h>

#include "lexer.h"
#include "bobo_bin.h"

#define CONSTANT_STRING 0

typedef struct {
    /* Imports */
    unsigned int import_count;
    char **imports;


} aux;

typedef struct {
    char *lexeme;
    u_int8_t type;
    union {

    };
} symbol;

typedef struct {
    aux *aux;
    token *prev;
    token *curr;
    lexer *lexer;
    bobo_bin *bin;
} translator;

translator *init_translator(lexer *lexer);
bobo_bin *translate(translator *translator);

#endif