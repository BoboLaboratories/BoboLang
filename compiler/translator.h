#ifndef BOBO_LANG_COMPILER_TRANSLATOR_H
#define BOBO_LANG_COMPILER_TRANSLATOR_H

#include <stdio.h>

typedef struct {
    int import_count;
    char **imports;
} AST_importlist;

typedef struct {
    AST_importlist *importlist;
} AST_file;

void translate(FILE *fptr);

#endif