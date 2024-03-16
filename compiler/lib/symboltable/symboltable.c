#include <malloc.h>
#include <string.h>
#include <stdarg.h>

#include "parser/ast.h"
#include "lang/binary/base.h"
#include "lib/symboltable/symboltable.h"
#include "lib/data/hashtable/hashtable.h"

struct symbol_table {
    HashTable *ht;
    SymbolTable *prev;
    unsigned int layer;

    u2 fun_addr;
    u2 var_addr;
};

/*
 *                              LAYER   NAME    TYPE    ADDR
 *
 * import io.net
 *
 * var k = 1                <-- 0       k       VAR     0
 *
 * fun main(a)              <-- 0       main    FUN     0
 * {                        <-- 1       a       VAR     0
 *      b = 10              <-- 1       b       VAR     1
 *      {
 *          c = 23          <-- 2       c       VAR     2
 *
 *          k = 50
 *      }
 * }
 *
 */

#define SCOPE_SEPARATOR '#'
#define ARITY_SEPARATOR '/'

static char *mk_key(SymbolType type, const char *name, u1 min_arity) {
    size_t key_len = 2; /* for SCOPE_SEPARATOR and '\0' */
    size_t index = 0;
    size_t name_len;

    if (type == FUN) {
        u1 min_args = min_arity; /* TODO */
        key_len += (min_args < 100) ? ((min_args < 10) ? 1 : 2) : 3;
        key_len += 1; /* for ARITY_SEPARATOR */
    }

    name_len = strlen(name);
    key_len += name_len;

    char *key = malloc(key_len);
    key[index++] = SCOPE_SEPARATOR;
    strcpy(&key[index], name);

    if (type == FUN) {
        index += name_len;
        key[index++] = ARITY_SEPARATOR;
        sprintf(&key[index], "%hhu", min_arity);
    }

    key[key_len - 1] = '\0';

    return key;
}

static u2 compute_address(SymbolTable *table, SymbolType type, const char *key) {
    if (table->layer > 1) {
        return compute_address(table->prev, type, key);
    }

    switch (type) {
    case VAR:
        return table->var_addr++;
    case FUN:
        return table->fun_addr++;
    }
}

SymbolTable *st_push(SymbolTable *prev) {
    SymbolTable *st = malloc(sizeof(SymbolTable));

    st->ht = ht_create();
    st->prev = prev;
    st->fun_addr = 0;
    st->var_addr = 0;

    if (prev == NULL) {
        st->layer = 0;
    } else {
        st->layer = prev->layer + 1;
    }

    return st;
}

SymbolTable *st_pop(SymbolTable *curr) {
    return curr->prev;
}

static Symbol *st_get_entry(SymbolTable *table, char *key) {
    Symbol *value = ht_get(table->ht, key);

    if (value == NULL && table->prev != NULL) {
        value = st_get_entry(table->prev, key);
    }

    return value;
}

Symbol *st_get(SymbolTable *table, SymbolType type, ...) {
    FunctionSignature *sig = NULL;
    char *name = NULL;
    u1 arity = 0;

    va_list args;
    va_start(args, type);
    switch (type) {
    case VAR:
        name = va_arg(args, char *);
        break;
    case FUN:
        sig = va_arg(args, FunctionSignature *);
        name = sig->name;
        arity = sig->args != NULL ? al_size(sig->args) : 0;
        break;
    }
    va_end(args);

    char *key = mk_key(type, name, arity);
    Symbol *symbol = st_get_entry(table, key);
    free(key);

    return symbol;
}

Symbol *st_set(SymbolTable *table, SymbolType type, ...) {
    const char *name;
    void *info;
    u1 arity = 0;

    va_list args;
    va_start(args, type);
    switch (type) {
    case VAR:
        info = va_arg(args, VariableSignature *);
        name = ((VariableSignature *) info)->name;
        break;
    case FUN:
        info = va_arg(args, FunctionSignature *);
        FunctionSignature * fun = info;
        name = fun->name;
        arity = fun->args != NULL ? al_size(fun->args) : 0;
        break;
    }
    va_end(args);

    char *key = mk_key(type, name, arity);
    Symbol *symbol = malloc(sizeof(Symbol));
    *symbol = (Symbol) {
            .address = compute_address(table, type, key),
            .scope = (table->layer == 0) ? GLOBAL : LOCAL,
            .type = type,
            .info = info
    };

    ht_set(table->ht, key, symbol);
    free(key);

    return symbol;
}