#ifndef BOBO_LANG_COMPILER_AST_H
#define BOBO_LANG_COMPILER_AST_H

#include <stdbool.h>
#include <sys/types.h>
#include "macrolist.h"

typedef struct {

} ast_expr;

typedef struct {

} ast_stat;

typedef struct {
    bool is_const;
    char *name;
    ast_expr *expr;
} ast_funarg;

typedef struct {
    bool is_private;
    bool is_native;
    char *name;
    LIST_DEF(ast_funarg *, u_int8_t, args);
} ast_fundef;

typedef struct {
    enum {
        STAT,
        FUNDEF
    } type;
    void *value;
} ast_program_stat;

typedef struct {
    LIST_DEF(char *, u_int32_t, imports);
    LIST_DEF(ast_program_stat *, u_int32_t, stats);
} ast_program;

#endif