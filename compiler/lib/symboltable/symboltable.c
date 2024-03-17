/*
 * MIT License
 *
 * Copyright (C) 2024 BoboLabs.net
 * Copyright (C) 2024 Mattia Mignogna (https://stami.bobolabs.net)
 * Copyright (C) 2024 Fabio Nebbia (https://glowy.bobolabs.net)
 * Copyright (C) 2024 Third party contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <malloc.h>
#include <string.h>
#include <stdarg.h>

#include "parser/tree.h"
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
 * var k = 1                <-- 0       k       SYM_VAR     0
 *
 * fun main(a)              <-- 0       main    SYM_FUN     0
 * {                        <-- 1       a       SYM_VAR     0
 *      b = 10              <-- 1       b       SYM_VAR     1
 *      {
 *          c = 23          <-- 2       c       SYM_VAR     2
 *
 *          k = 50
 *      }
 * }
 *
 */

#define SCOPE_SEPARATOR '#'
#define ARITY_SEPARATOR '/'

static char *mk_key(SymbolType type, const char *name, u1 arity) {
    size_t key_len = 2; /* for SCOPE_SEPARATOR and '\0' */
    size_t index = 0;
    size_t name_len;

    if (type == SYM_FUN) {
        u1 min_args = arity;
        key_len += (min_args < 100) ? ((min_args < 10) ? 1 : 2) : 3;
        key_len += 1; /* for ARITY_SEPARATOR */
    }

    name_len = strlen(name);
    key_len += name_len;

    char *key = malloc(key_len);
    key[index++] = SCOPE_SEPARATOR;
    strcpy(&key[index], name);

    if (type == SYM_FUN) {
        index += name_len;
        key[index++] = ARITY_SEPARATOR;
        sprintf(&key[index], "%hhu", arity);
    }

    key[key_len - 1] = '\0';

    return key;
}

static u2 compute_address(SymbolTable *table, SymbolType type, const char *key) {
    if (table->layer > 1) {
        return compute_address(table->prev, type, key);
    }

    switch (type) {
    case SYM_VAR:
        return table->var_addr++;
    case SYM_FUN:
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
    FunDefSignature *sig = NULL;
    char *name = NULL;
    u1 arity = 0;

    va_list args;
    va_start(args, type);
    switch (type) {
    case SYM_VAR:
        name = va_arg(args, char *);
        break;
    case SYM_FUN:
        sig = va_arg(args, FunDefSignature *);
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
    case SYM_VAR:
        info = va_arg(args, VarDeclSignature *);
        name = ((VarDeclSignature *) info)->name;
        break;
    case SYM_FUN:
        info = va_arg(args, FunDefSignature *);
        FunDefSignature * fun = info;
        name = fun->name;
        arity = fun->args != NULL ? fun->min_args_count : 0;
        break;
    }
    va_end(args);

    char *key = mk_key(type, name, arity);
    Symbol *symbol = malloc(sizeof(Symbol));
    *symbol = (Symbol) {
            .address = compute_address(table, type, key),
            .scope = (table->layer == 0) ? GLOBAL : LOCAL,
            .type = type,
            .info = info,
            .key = key
    };

    ht_set(table->ht, key, symbol);

    return symbol;
}