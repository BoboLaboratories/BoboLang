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

#ifndef BOBO_LANG_COMPILER_PARSE_TREE_H
#define BOBO_LANG_COMPILER_PARSE_TREE_H

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
} VarDeclSignature;

typedef struct {
    bool is_const;
    char *name;
} FunArgSignature;

typedef struct {
    bool is_private;
    bool is_native;
    char *name;
    u1 min_args_count;
    ArrayList *args;
} FunDefSignature;

typedef enum {
    EXPR_QID,
    EXPR_INVOKE,
    EXPR_NUMERIC_LITERAL
} ExprType;

typedef enum {
    STAT_INVOKE,
    STAT_VAR_DECL,
    STAT_VAR_ASSIGN
} StatType;

typedef enum {
    PROGRAM_STAT,
    PROGRAM_FUNDEF
} ProgramStatType;


/*
 * Parse tree nodes
 */

typedef struct {
    char *fun;
    ArrayList *args;
} PT_Invoke;

typedef struct {
    ExprType type;
    void *expr;
} PT_Expr;

typedef struct {
    VarDeclSignature *var;
    PT_Expr *init;
} PT_StatVarDecl;

typedef struct {
    char *var;
    PT_Expr *expr;
} PT_StatVarAssign;

typedef struct {
    StatType type;
    void *value;
} PT_Stat;

typedef struct {
    FunArgSignature *arg;
    PT_Expr *init;
} PT_FunArg;

typedef struct {
    FunDefSignature *fun;
    ArrayList *stats;
} PT_FunDef;

typedef struct {
    ProgramStatType type;
    void *value;
} PT_ProgramStat;

typedef struct {
    ArrayList *imports;
    ArrayList *stats;
} PT_Program;

#endif