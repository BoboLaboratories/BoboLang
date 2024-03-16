#ifndef BOBO_LANG_COMPILER_LIB_SYMBOLTABLE_H
#define BOBO_LANG_COMPILER_LIB_SYMBOLTABLE_H

typedef enum {
    VAR,
    FUN
} SymbolType;

typedef enum {
    GLOBAL,
    LOCAL
} Scope;

typedef struct {
    SymbolType type;
    Scope scope;
    u2 address;
    void *info;
} Symbol;

typedef struct symbol_table SymbolTable;

SymbolTable *st_push(SymbolTable *prev);

SymbolTable *st_pop(SymbolTable *curr);

Symbol *st_get(SymbolTable *table, SymbolType type, ...);

Symbol *st_set(SymbolTable *table, SymbolType type, ...);

#endif