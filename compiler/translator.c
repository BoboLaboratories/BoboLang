#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "lexer.h"
#include "compiler.h"
#include "translator.h"
#include "lib/console.h"

#define dynamic_add(dest, count, type, val)             \
        count++;                                        \
        dest = reallocarray(dest, count, sizeof(type)); \
        dest[count - 1] = val;

#define function_add(fun) dynamic_add(t->bin->functions, t->bin->function_count, function, fun)

static void qidexprp(translator *t);

static void move(translator *t) {
    /* if (token_lexeme_needs_freeing(t->prev)) {
        free(t->prev->lexeme);
    }*/
    memcpy(t->prev, t->curr, sizeof(token));
    *t->curr = scan(t->lexer);
    TOK_PRINT(t->curr);
}

static void match(translator *t, int tag) {
    if (t->curr->tag == tag) {
        if (t->curr->tag != EOF) {
            move(t);
        }
    } else {
        print(E, "--- unexpected token, expected %d\n", tag);
        TOK_PRINT(t->curr);
        print(E, "---");
    }
}

static char *qidp(translator *t, char *id) {
    switch (t->curr->tag) {
    case DOT:
        match(t, DOT);
        strcat(id, ".");
        match(t, ID);
        id = realloc(id, strlen(id) + 1 /* for . */ + strlen(t->prev->lexeme) + 1 /* for \0 */);
        strcat(id, t->prev->lexeme);
        qidp(t, id);
        break;
    case PRIVATE:
    case ASSIGN:
    case IMPORT:
    case NATIVE:
    case COMMA:
    case CONST:
    case EOF:
    case FUN:
    case LPT:
    case RPT:
    case RPG:
    case VAR:
    case ID:
        return id;
    default:
        print(E, "qidp");
        exit(EXIT_FAILURE);
    }
}

static char *qid(translator *t) {
    switch (t->curr->tag) {
    case ID:
        match(t, ID);
        char *id = t->prev->lexeme;
        return qidp(t, id);
    default:
        print(E, "qid");
        exit(EXIT_FAILURE);
    }
}

static void expr(translator *t) {
    switch (t->curr->tag) {
    case ID:
        qid(t);
        qidexprp(t);
        break;
    default:
        print(E, "expr");
        exit(EXIT_FAILURE);
    }
}

static void invokearglistp(translator *t) {
    switch (t->curr->tag) {
    case COMMA:
        match(t, COMMA);
        expr(t);
        invokearglistp(t);
        break;
    case RPT:
        break;
    default:
        print(E, "invokearglistp");
        exit(EXIT_FAILURE);
    }
}

static void invokearglist(translator *t) {
    switch (t->curr->tag) {
    case ID:
        expr(t);
        invokearglistp(t);
        break;
    case RPT:
        break;
    default:
        print(E, "invokearglist");
        exit(EXIT_FAILURE);
    }
}

static void invokeargs(translator *t) {
    switch (t->curr->tag) {
    case LPT:
        match(t, LPT);
        invokearglist(t);
        match(t, RPT);
        break;
    default:
        print(E, "invokeargs");
        exit(EXIT_FAILURE);
    }
}

static void qidexprp(translator *t) {
    switch (t->curr->tag) {
    case LPT:
        invokeargs(t);
        break;
    case PRIVATE:
    case NATIVE:
    case COMMA:
    case CONST:
    case EOF:
    case FUN:
    case RPG:
    case RPT:
    case VAR:
    case ID:
        break;
    default:
        print(E, "qidexprp");
        exit(EXIT_FAILURE);
    }
}

static void assign(translator *t) {
    switch (t->curr->tag) {
    case ASSIGN:
        match(t, ASSIGN);
        expr(t);
        break;
    default:
        print(E, "assign");
        exit(EXIT_FAILURE);
    }
}

static void qidstatp(translator *t) {
    switch (t->curr->tag) {
    case ASSIGN:
        assign(t);
        break;
    case LPT:
        invokeargs(t);
        break;
    default:
        print(E, "qidstatp");
        exit(EXIT_FAILURE);
    }
}

static void statvardecl(translator *t) {
    switch (t->curr->tag) {
    case CONST:
        match(t, CONST);
        match(t, ID);
        assign(t);
        break;
    case VAR:
        match(t, VAR);
        match(t, ID);
        assign(t);
        break;
    default:
        print(E, "statvardecl");
        exit(EXIT_FAILURE);
    }
}

static void stat(translator *t) {
    switch (t->curr->tag) {
    case CONST:
    case VAR:
        statvardecl(t);
        break;
    case ID:
        qid(t);
        qidstatp(t);
        break;
    default:
        print(E, "stat");
        exit(EXIT_FAILURE);
    }
}

static void statlistp(translator *t) {
    switch (t->curr->tag) {
    case CONST:
    case VAR:
    case ID:
        stat(t);
        statlistp(t);
        break;
    case RPG:
        break;
    default:
        print(E, "statlistp");
        exit(EXIT_FAILURE);
    }
}

static void statlist(translator *t) {
    switch (t->curr->tag) {
    case LPG:
        match(t, LPG);
        statlistp(t);
        match(t, RPG);
        break;
    default:
        print(E, "statlist");
        exit(EXIT_FAILURE);
    }
}

static void funarg(translator *t) {
    switch (t->curr->tag) {
    case CONST:
        match(t, CONST);
        match(t, ID);
        break;
    case ID:
        match(t, ID);
        break;
    default:
        print(E, "funarg");
        exit(EXIT_FAILURE);
    }
}

static void fundefarglistp(translator *t, function *fun) {
    switch (t->curr->tag) {
    case COMMA:
        fun->max_args++;
        match(t, COMMA);
        funarg(t);
        assign(t);
        fundefarglistp(t, fun);
        break;
    case RPT:
        fun->max_args += fun->min_args;
        break;
    default:
        print(E, "fundefarglistp");
        exit(EXIT_FAILURE);
    }
}

static void funarglistp(translator *t, function *fun) {
    switch (t->curr->tag) {
    case COMMA:
        fun->min_args++;
        match(t, COMMA);
        funarg(t);
        funarglistp(t, fun);
        break;
    case ASSIGN:
        fun->max_args++;
        assign(t);
        fundefarglistp(t, fun);
        break;
    case RPT:
        fun->min_args++;
        fun->max_args += fun->min_args;
        break;
    default:
        print(E, "funarglistp");
        exit(EXIT_FAILURE);
    }
}

static void funarglist(translator *t, function *fun) {
    switch (t->curr->tag) {
    case CONST:
    case ID:
        funarg(t);
        funarglistp(t, fun);
        break;
    case RPT:
        break;
    default:
        print(E, "funarglist");
        exit(EXIT_FAILURE);
    }
}

static function *funsig(translator *t) {
    switch (t->curr->tag) {
    case FUN: {
        match(t, FUN);
        match(t, ID);
        function *fun = malloc(sizeof(function));
        fun->name = t->prev->lexeme;
        fun->mod = FUN_MOD_BASE;
        fun->min_args = fun->max_args = 0;
        match(t, LPT);
        funarglist(t, fun);
        match(t, RPT);
        return fun;
    }
    default:
        print(E, "funsig");
        exit(EXIT_FAILURE);
    }
}

static function *fundef(translator *t) {
    switch (t->curr->tag) {
    case FUN: {
        function *fun = funsig(t);
        statlist(t);
        return fun;
    }
    default:
        print(E, "fundef");
        exit(EXIT_FAILURE);
    }
}

static function *nativefundef(translator *t) {
    switch (t->curr->tag) {
    case NATIVE: {
        match(t, NATIVE);
        function *fun = funsig(t);
        SET_BIT(fun->mod, FUN_MOD_NATIVE);
        return fun;
    }
    default:
        print(E, "nativefundef");
        exit(EXIT_FAILURE);
    }
}

static void privatefilep(translator *t) {
    switch (t->curr->tag) {
    case CONST:
    case VAR:
        statvardecl(t);
        break;
    case NATIVE: {
        function *fun = nativefundef(t);
        SET_BIT(fun->mod, FUN_MOD_PRIVATE);
        function_add(fun);
        break;
    }
    case FUN: {
        function *fun = fundef(t);
        SET_BIT(fun->mod, FUN_MOD_PRIVATE);
        function_add(fun);
        break;
    }
    default:
        print(E, "privatefilep");
        exit(EXIT_FAILURE);
    }
}

static void filep(translator *t) {
    switch (t->curr->tag) {
    case PRIVATE:
        match(t, PRIVATE);
        privatefilep(t);
        filep(t);
        break;
    case NATIVE: {
        function *fun = nativefundef(t);
        function_add(fun);
        filep(t);
        break;
    }
    case FUN: {
        function *fun = fundef(t);
        function_add(fun);
        filep(t);
        break;
    }
    case CONST:
    case VAR:
    case ID:
        stat(t);
        filep(t);
        break;
    case EOF:
        break;
    default:
        print(E, "filep");
        exit(EXIT_FAILURE);
    }
}

static void importlist(translator *t) {
    switch (t->curr->tag) {
    case IMPORT: {
        match(t, IMPORT);
        char *id = qid(t);

        t->aux->import_count++;
        t->aux->imports = reallocarray(t->aux->imports, t->aux->import_count, sizeof(char *));
        t->aux->imports[t->aux->import_count - 1] = id;

        importlist(t);
        break;
    }
    case PRIVATE:
    case NATIVE:
    case CONST:
    case EOF:
    case FUN:
    case VAR:
    case ID:
        break;
    default:
        print(E, "importlist");
        exit(EXIT_FAILURE);
    }
}

static void file(translator *t) {
    switch (t->curr->tag) {
    case PRIVATE:
    case NATIVE:
    case IMPORT:
    case CONST:
    case EOF:
    case FUN:
    case VAR:
    case ID:
        importlist(t);
        filep(t);
        match(t, EOF);
        break;
    default:
        print(E, "file");
        exit(EXIT_FAILURE);
    }
}


translator *init_translator(lexer *lexer) {
    translator *t = malloc(sizeof(translator));

    t->lexer = lexer;
    t->prev = malloc(sizeof(token));
    t->curr = malloc(sizeof(token));

    t->bin = malloc(sizeof(bobo_bin));

    t->bin->magic = BOBO_LANG_MAGIC;
    t->bin->major_version = BOBO_LANG_MAJOR;
    t->bin->minor_version = BOBO_LANG_MINOR;

    t->bin->constant_count = 0;
    t->bin->constants = NULL;

    t->bin->function_count = 0;
    t->bin->functions = NULL;

    t->aux = malloc(sizeof(aux));
    t->aux->import_count = 0;
    t->aux->imports = NULL;

    return t;
}

bobo_bin *translate(translator *t) {
    move(t);
    file(t);

    bobo_bin *bin = t->bin;
    free(t->lexer);
    free(t->prev);
    free(t->curr);
    free(t->aux);
    free(t);

    return bin;
}
