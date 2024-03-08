#ifndef BOBO_LANG_COMPILER_AST_H
#define BOBO_LANG_COMPILER_AST_H

#include <stdbool.h>
#include <sys/types.h>

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
    u_int8_t arg_count;
    ast_funarg *args;
} ast_fundef;

typedef struct {
    enum {
        STAT,
        FUNDEF
    } type;
    void *value;
} ast_program_stat;

typedef struct {
    unsigned int import_count;
    char **imports;
} ast_import_list;

typedef struct {
    ast_import_list *import_list;
    unsigned int stat_count;
    ast_program_stat **stats;
} ast_program;

#endif