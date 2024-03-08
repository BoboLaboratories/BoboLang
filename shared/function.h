#ifndef BOBO_LANG_FUNCTION_H
#define BOBO_LANG_FUNCTION_H

#include "sys/types.h"

#define FUN_MOD_BASE_VALUE  0

#define FUN_MOD_BIT_PRIVATE 0
#define FUN_MOD_BIT_NATIVE  1

typedef u_int8_t funmod;

typedef struct {
    char *name;
    funmod mod;
    u_int8_t min_args;
    u_int8_t max_args;
    u_int32_t *code;
} function;

#endif