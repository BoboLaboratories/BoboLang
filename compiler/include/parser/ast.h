#ifndef BOBO_LANG_COMPILER_AST_H
#define BOBO_LANG_COMPILER_AST_H

#include <stdbool.h>
#include <sys/types.h>

#include "lib/data/arraylist/arraylist.h"

typedef struct {
    char *qid;
    ArrayList *args;
} AST_ExprInvoke;

typedef struct {
    enum {
        EXPR_QID,
        EXPR_INVOKE,
        EXPR_NUMERIC_LITERAL
    } type;
    void *value;
} AST_Expr;

typedef struct {
    char *qid;
    ArrayList *args;
} AST_StatInvoke;

typedef struct {
    bool is_private;
    bool is_const;
    char *name;
} VariableSignature;

typedef struct {
    VariableSignature *sig;
    AST_Expr *init;
} AST_StatVarDecl;

typedef struct {
    char *name;
    AST_Expr *expr;
} AST_StatVarAssign;

typedef struct {
    enum {
        STAT_INVOKE,
        STAT_VAR_DECL,
        STAT_VAR_ASSIGN
    } type;
    void *value;
} AST_Stat;

typedef struct {
    bool is_const;
    char *name;
    AST_Expr *expr;
} AST_FunArg;

typedef struct {
    bool is_private;
    bool is_native;
    char *name;
    ArrayList *args;
} FunctionSignature;

typedef struct {
    FunctionSignature *sig;
    ArrayList *stats;
} AST_FunDef;

typedef struct {
    enum {
        PROGRAM_STAT,
        PROGRAM_FUNDEF
    } type;
    void *value;
} AST_ProgramStat;

typedef struct {
    ArrayList *imports;
    ArrayList *stats;
} AST_Program;

#endif