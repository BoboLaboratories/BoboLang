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

struct bobo_bin {
    /* Unix file info: 0xB0B0B0B0 */
    u_int32_t magic;

    /* compilation data */
    u_int16_t major_version;
    u_int16_t minor_version;

    /* module description */
    u_int8_t access_flags;
};


#endif