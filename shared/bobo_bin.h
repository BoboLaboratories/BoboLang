#ifndef BOBO_LANG_BIN_H
#define BOBO_LANG_BIN_H

#include "sys/types.h"

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

typedef struct {
    u_int8_t type;
    union {
        char *str;
    } value;
} constant;

typedef u_int8_t funmod;

typedef struct {
    char *name;
    funmod mod;
    u_int8_t min_args;
    u_int8_t max_args;
    u_int32_t *code;
} function;

typedef struct {
    /* Unix file info */
    u_int32_t magic;

    /* Compilation data */
    u_int16_t major_version;
    u_int16_t minor_version;

    /* Constants */
    u_int16_t constant_count;
    constant *constants;

    /* Functions */
    u_int16_t function_count;
    function **functions;
} bobo_bin;

#endif