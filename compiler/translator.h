#ifndef BOBO_LANG_COMPILER_TRANSLATOR_H
#define BOBO_LANG_COMPILER_TRANSLATOR_H

#include <stdio.h>

#include "lexer.h"

#define CONSTANT_STRING 0

typedef struct {
    unsigned char type;
    union {
        char *str;
    } value;
} constant;

typedef struct {
    unsigned int import_count;
    char **imports;
    unsigned int constant_count;
    constant *constants;
} bobo_bin;

typedef struct {
    lexer *lexer;
    token *prev;
    token *look;
    bobo_bin *bin;
} translator;

translator init_translator(lexer *lexer);
bobo_bin *translate(translator *translator);

#endif