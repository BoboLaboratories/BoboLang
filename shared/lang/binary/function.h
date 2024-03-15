#ifndef BOBO_LANG_FUNCTION_H
#define BOBO_LANG_FUNCTION_H

#include "base.h"

#define FUN_MOD_BASE_VALUE  0

#define FUN_MOD_BIT_PRIVATE 0
#define FUN_MOD_BIT_NATIVE  1

typedef struct {
    char *name;
    u1 mod;
    u1 min_args;
    u1 max_args;
    u4 code_size;
    void **code;
} Function;

#endif