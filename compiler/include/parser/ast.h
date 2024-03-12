#ifndef BOBO_LANG_COMPILER_AST_H
#define BOBO_LANG_COMPILER_AST_H

#include <stdbool.h>
#include <sys/types.h>

#include "lib/structures/list.h"

typedef struct {
    char *qid;
} ast_expr_qid;

typedef struct {
    char *qid;
    List *args;
} ast_expr_invoke;

typedef struct {
    enum {
        EXPR_NUMERIC_LITERAL_DOUBLE,
        EXPR_NUMERIC_LITERAL_INTEGER
    } type;
    u_int64_t value;
} ast_expr_numeric_literal;

typedef struct {
    enum {
        EXPR_QID,
        EXPR_INVOKE,
        EXPR_NUMERIC_LITERAL
    } type;
    void *value;
} ast_expr;

typedef struct {
    char *qid;
    List *args;
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
    List *args;
} ast_fundef;

typedef struct {
    enum {
        PROGRAM_STAT,
        PROGRAM_FUNDEF
    } type;
    void *value;
} ast_program_stat;

typedef struct {
    List *imports;
    List *stats;
} ast_program;

#endif