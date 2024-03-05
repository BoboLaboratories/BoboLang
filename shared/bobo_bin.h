#ifndef BOBO_LANG_BIN_H
#define BOBO_LANG_BIN_H

#include "sys/types.h"

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

#define MAGIC 0xB0B0B0B0

typedef struct {
    u_int8_t type;
    union {
        char *str;
    } value;
} constant;

typedef struct {
    /* Unix file info: 0xB0B0B0B0 */
    u_int32_t magic;

    /* compilation data */
    u_int16_t major_version;
    u_int16_t minor_version;

    /* constants */
    u_int16_t constant_count;
    constant *constants;
} bobo_bin;

#endif