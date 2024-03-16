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

#ifndef BOBO_LANG_BIN_H
#define BOBO_LANG_BIN_H

#include "base.h"
#include "variable.h"
#include "function.h"

#define FUN_MOD_BASE    0

#define FUN_MOD_PRIVATE 0
#define FUN_MOD_NATIVE  1

#define CHECK_BIT(var, pos)  (((var) >> (pos)) & 1)
#define SET_BIT(var, pos)    var = ((var) | (1 << (pos)))

/*
    - nomi dei moduli importanti
    - nome del modulo corrente
    - dati:
        - modificatori (private, const)
        - nome
        - codice per la loro inizializzazione
    - funzioni:
        - modificatori (private, native)
        - nome
        - argomenti (con modificatori, con default)
    - area delle costanti
*/

#define BOBO_LANG_MAGIC 0xB0B0B0B0

#define BOBO_LANG_MAJOR 0
#define BOBO_LANG_MINOR 1

typedef struct {
    u_int8_t type;
    union {
        char *str;
    } value;
} constant;

typedef struct {
    /* Unix file info */
    u4 magic;

    /* Compilation data */
    u2 major_version;
    u2 minor_version;

    /* Constants */
    u2 constant_count;
    constant *constants;

    /* Variable */
    u2 variable_count;
    Variable variables;

    /* Functions */
    u2 function_count;
    Function **functions;
} BinaryModule;

#endif