#ifndef BOBO_LANG_COMPILER_LIB_SYMBOLTABLE_H
#define BOBO_LANG_COMPILER_LIB_SYMBOLTABLE_H


typedef struct symbol_table SymbolTable;

SymbolTable *st_push(SymbolTable *prev);

SymbolTable *st_pop(SymbolTable *curr);

void *st_get(SymbolTable *table, const char *key);

const char *st_set(SymbolTable *table, const char *key, void *value);

#endif