#ifndef BOBO_LANG_VARIABLE_H
#define BOBO_LANG_VARIABLE_H

#include "base.h"

#define VAR_MOD_BASE_VALUE  0

#define VAR_MOD_BIT_PRIVATE 0
#define VAR_MOD_BIT_CONST   1

typedef struct {
    char *name;
    u1 mod;
    char *scope;
} Variable;

#endif