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

#include "parser/tree.h"
#include "lib/symboltable/symboltable.h"

/*
 * AST nodes
 */
typedef struct {
    Symbol *symbol;
    FunDefSignature *fun;
    ArrayList *args;
} AST_Invoke;

typedef struct {
    ExprType type;
    void *expr;
} AST_Expr;

typedef struct {
    Symbol *symbol;
    VarDeclSignature *var;
    AST_Expr *init;
} AST_StatVarDecl;

typedef struct {
    Symbol *symbol;
    VarDeclSignature *var;
    AST_Expr *expr;
} AST_StatVarAssign;

typedef struct {
    StatType type;
    void *value;
} AST_Stat;

typedef struct {
    Symbol *symbol;
    FunArgSignature *arg;
    AST_Expr *init;
} AST_FunArg;

typedef struct {
    Symbol *fun;
    ArrayList *stats;
} AST_FunDef;

typedef struct {
    ProgramStatType type;
    void *value;
} AST_ProgramStat;

typedef struct {
    ArrayList *imports;
    ArrayList *stats;
} AST_Program;

#endif