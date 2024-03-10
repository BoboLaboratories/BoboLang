#ifndef BOBO_LANG_COMPILER_AST_H
#define BOBO_LANG_COMPILER_AST_H

#include <stdbool.h>
#include <sys/types.h>
#include "macrolist.h"

typedef struct {
    char *qid;
} ast_expr_qid;

typedef struct {
    char *qid;
    /* TODO: arglist */
} ast_expr_invoke;

typedef struct {
    enum {
        EXPR_QID,
        EXPR_INVOKE
    } type;
    void *value;
} ast_expr;

typedef struct {
    char *qid;
} ast_stat_invoke;

typedef struct {
    bool is_private;
    bool is_const;
    char *name;
    ast_expr *init;
} ast_stat_var_decl;

typedef struct {
    char *qid;
    ast_expr *expr;
} ast_stat_var_assign;

typedef struct {
    enum {
        STAT_INVOKE,
        STAT_VAR_DECL,
        STAT_VAR_ASSIGN
    } type;
    void *value;
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
        PROGRAM_STAT,
        PROGRAM_FUNDEF
    } type;
    void *value;
} ast_program_stat;

typedef struct {
    LIST_DEF(char *, u_int32_t, imports);
    LIST_DEF(void *, u_int32_t, stats);
} ast_program;

#endif