#ifndef BOBO_LANG_VARIABLE_H
#define BOBO_LANG_VARIABLE_H

#include "sys/types.h"

#define VAR_MOD_BASE_VALUE  0
#define VAR_MOD_BIT_PRIVATE 0
#define VAR_MOD_BIT_CONST   1

typedef u_int8_t varmod;

typedef struct {
    char *name;
    varmod mod;
    char *scope;
} var;

#endif