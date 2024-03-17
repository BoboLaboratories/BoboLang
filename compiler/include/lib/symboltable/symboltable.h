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

#ifndef BOBO_LANG_COMPILER_LIB_SYMBOLTABLE_H
#define BOBO_LANG_COMPILER_LIB_SYMBOLTABLE_H

typedef enum {
    SYM_VAR,
    SYM_FUN
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
    char *key;
} Symbol;

typedef struct symbol_table SymbolTable;

SymbolTable *st_push(SymbolTable *prev);

SymbolTable *st_pop(SymbolTable *curr);

Symbol *st_get(SymbolTable *table, SymbolType type, ...);

Symbol *st_set(SymbolTable *table, SymbolType type, ...);

#endif