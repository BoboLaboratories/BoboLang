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