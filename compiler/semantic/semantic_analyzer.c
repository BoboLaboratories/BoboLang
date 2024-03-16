#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "lib/console/console.h"
#include "semantic/semantic_analyzer.h"
#include "lib/symboltable/symboltable.h"
#include "lib/data/arraylist/arraylist.h"

struct semantic_analyzer {
    AST_Program *ast;
    SymbolTable *st;
    Meta *meta;
};

static void pass1_stat(SemanticAnalyzer *a, AST_Stat *node) {
    switch (node->type) {
    case STAT_VAR_DECL: {
        AST_StatVarDecl *stat = node->value;
        if (st_get(a->st, VAR, stat->sig->name)) {
            printf("[%s:%d] error: variable %s is already defined\n", a->meta->pathname, 0, stat->sig->name);
            exit(EXIT_FAILURE);
        }
        st_set(a->st, VAR, stat->sig);
        break;
    }
    case STAT_VAR_ASSIGN: {
        AST_StatVarAssign *stat = node->value;
        Symbol *symbol = st_get(a->st, VAR, stat->name);
        VariableSignature *desc = symbol->info;
        if (desc == NULL) {
            printf("[%s:%d] error: unknown symbol %s\n", a->meta->pathname, 0, stat->name);
            exit(EXIT_FAILURE);
        } else if (desc->is_const) {
            printf("[%s:%d] error: cannot reassign constant variable %s\n", a->meta->pathname, 0, stat->name);
            exit(EXIT_FAILURE);
        }
        break;
    }
    default:
        break;
    }
}

static bool is_fun_sig_equal(FunctionSignature *a, FunctionSignature *b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    if (strcmp(a->name, b->name) != 0) {
        return false;
    }

    if ((a->args == NULL && b->args != NULL) || (a->args != NULL && b->args == NULL)) {
        return false;
    }

    if (a->args == NULL) {
        return true;
    }

    return al_size(a->args) == al_size(b->args);
}

static void pass1_fun_def(SemanticAnalyzer *a, AST_FunDef *fun) {
    Symbol *symbol = st_get(a->st, FUN, fun->sig);
    if (symbol != NULL && is_fun_sig_equal(fun->sig, symbol->info)) {
        printf("[%s:%d] error: function %s cannot be redefined\n", a->meta->pathname, 0, fun->sig->name);
        exit(EXIT_FAILURE);
    }
    st_set(a->st, FUN, fun->sig);
}

static void pass1(SemanticAnalyzer *a) {
    ArrayListIterator it = al_iterator(a->ast->stats);
    AST_ProgramStat *node;

    while ((node = al_iterator_next(&it))) {
        switch (node->type) {
        case PROGRAM_STAT:
            pass1_stat(a, node->value);
            break;
        case PROGRAM_FUNDEF:
            pass1_fun_def(a, node->value);
            break;
        }
    }
}

SemanticAnalyzer *init_semantic_analyzer(Meta *meta, AST_Program *ast) {
    SemanticAnalyzer *sem = malloc(sizeof(SemanticAnalyzer));

    sem->st = st_push(NULL);
    sem->meta = meta;
    sem->ast = ast;

    return sem;
}

void analyze(SemanticAnalyzer *analyzer) {
    pass1(analyzer);
}