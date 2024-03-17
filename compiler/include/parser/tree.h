/*
 * MIT License
 *
 * Copyright (C) 2024 BoboLabs.net
 * Copyright (C) 2024 Mattia Mignogna (https://stami.bobolabs.net)
 * Copyright (C) 2024 Fabio Nebbia (https://glowy.bobolabs.net)
 * Copyright (C) 2024 Third party contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef BOBO_LANG_COMPILER_AST_H
#define BOBO_LANG_COMPILER_AST_H

#include <stdbool.h>
#include <sys/types.h>

#include "lang/binary/base.h"
#include "lib/data/arraylist/arraylist.h"

/*
 * Helper data structures
 */
typedef struct {
    bool is_private;
    bool is_const;
    char *name;
} VariableSignature;

typedef struct {
    bool is_private;
    bool is_native;
    char *name;
    u1 min_args;
    ArrayList *args;
} FunctionSignature;

/*
 * AST nodes
 */
typedef struct {
    void *fun;
    ArrayList *args;
} AST_Invoke;

typedef struct {
    enum {
        EXPR_QID,
        EXPR_INVOKE,
        EXPR_NUMERIC_LITERAL
    } type;
    void *expr;
} AST_Expr;

typedef struct {
    void *var;
    AST_Expr *init;
} AST_StatVarDecl;

typedef struct {
    void *var;
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
    void *arg;
    AST_Expr *expr;
} AST_FunArg;

typedef struct {
    void *fun;
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