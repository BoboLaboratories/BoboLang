#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "macrolist.h"
#include "lexer/lexer.h"
#include "parser/ast.h"
#include "parser/parser.h"
#include "lib/console.h"
#include "lib/string_utils.h"

struct parser {
    lexer *lexer;
    token *prev;
    token *curr;
    ast_program *program;
};


static void qidexprp(parser *p);

static void move(parser *p) {
    if (p->prev) {
        if (p->prev->tag == ID) {
            free(p->prev->lexeme);
        }
        free(p->prev);
    }
    p->prev = p->curr;
    p->curr = scan(p->lexer);
    TOK_PRINT(p->curr);
}

static void match(parser *p, int tag) {
    if (p->curr->tag == tag) {
        if (p->curr->tag != EOF) {
            move(p);
        }
    } else {
        print(E, "--- unexpected token, expected %d\n", tag);
        TOK_PRINT(p->curr);
        print(E, "---");
    }
}

static char *qidp(parser *p, char *id) {
    switch (p->curr->tag) {
    case DOT:
        match(p, DOT);
        match(p, ID);
        id = realloc(id, strlen(id) + 1 /* for . */ + strlen(p->prev->lexeme) + 1 /* for \0 */);
        strcat(id, ".");
        strcat(id, p->prev->lexeme);
        qidp(p, id);
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

static char *qid(parser *p) {
    switch (p->curr->tag) {
    case ID:
        match(p, ID);
        char *id = strdup(p->prev->lexeme);
        return qidp(p, id);
    default:
        print(E, "qid");
        exit(EXIT_FAILURE);
    }
}

static void expr(parser *p) {
    switch (p->curr->tag) {
    case ID:
        qid(p);
        qidexprp(p);
        break;
    default:
        print(E, "expr");
        exit(EXIT_FAILURE);
    }
}

static void invokearglistp(parser *p) {
    switch (p->curr->tag) {
    case COMMA:
        match(p, COMMA);
        expr(p);
        invokearglistp(p);
        break;
    case RPT:
        break;
    default:
        print(E, "invokearglistp");
        exit(EXIT_FAILURE);
    }
}

static void invokearglist(parser *p) {
    switch (p->curr->tag) {
    case ID:
        expr(p);
        invokearglistp(p);
        break;
    case RPT:
        break;
    default:
        print(E, "invokearglist");
        exit(EXIT_FAILURE);
    }
}

static void invokeargs(parser *p) {
    switch (p->curr->tag) {
    case LPT:
        match(p, LPT);
        invokearglist(p);
        match(p, RPT);
        break;
    default:
        print(E, "invokeargs");
        exit(EXIT_FAILURE);
    }
}

static void qidexprp(parser *p) {
    switch (p->curr->tag) {
    case LPT:
        invokeargs(p);
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

static ast_expr *assign(parser *p) {
    switch (p->curr->tag) {
    case ASSIGN:
        match(p, ASSIGN);
        expr(p);
        return NULL; /* TODO */
    default:
        print(E, "assign");
        exit(EXIT_FAILURE);
    }
}

static void qidstatp(parser *p) {
    switch (p->curr->tag) {
    case ASSIGN:
        assign(p);
        break;
    case LPT:
        invokeargs(p);
        break;
    default:
        print(E, "qidstatp");
        exit(EXIT_FAILURE);
    }
}

static void statvardecl(parser *p) {
    switch (p->curr->tag) {
    case CONST: {
        match(p, CONST);
        match(p, ID);
        assign(p);
        break;
    }
    case VAR:
        match(p, VAR);
        match(p, ID);
        assign(p);
        break;
    default:
        print(E, "statvardecl");
        exit(EXIT_FAILURE);
    }
}

static void stat(parser *p) {
    switch (p->curr->tag) {
    case CONST:
    case VAR:
        statvardecl(p);
        break;
    case ID:
        qid(p);
        qidstatp(p);
        break;
    default:
        print(E, "stat");
        exit(EXIT_FAILURE);
    }
}

static void statlistp(parser *p) {
    switch (p->curr->tag) {
    case CONST:
    case VAR:
    case ID:
        stat(p);
        statlistp(p);
        break;
    case RPG:
        break;
    default:
        print(E, "statlistp");
        exit(EXIT_FAILURE);
    }
}

static void statlist(parser *p) {
    switch (p->curr->tag) {
    case LPG:
        match(p, LPG);
        statlistp(p);
        match(p, RPG);
        break;
    default:
        print(E, "statlist");
        exit(EXIT_FAILURE);
    }
}

static ast_funarg *funarg(parser *p) {
    switch (p->curr->tag) {
    case CONST:
    case ID: {
        ast_funarg *arg = malloc(sizeof(ast_funarg));
        arg->is_const = p->curr->tag == CONST;
        if (arg->is_const) {
            match(p, CONST);
        }
        match(p, ID);
        arg->name = strdup(p->prev->lexeme);
        arg->expr = NULL;
        return arg;
    }
    default:
        print(E, "funarg");
        exit(EXIT_FAILURE);
    }
}

static void fundefarglistp(parser *p, ast_fundef *fun) {
    switch (p->curr->tag) {
    case COMMA:
        match(p, COMMA);
        ast_funarg *arg = funarg(p);
        arg->expr = assign(p);
        LIST_ADD(fun->args, arg);
        fundefarglistp(p, fun);
        break;
    case RPT:
        break;
    default:
        print(E, "fundefarglistp");
        exit(EXIT_FAILURE);
    }
}

static void funarglistp(parser *p, ast_fundef *fun, ast_funarg *arg) {
    LIST_ADD(fun->args, arg);

    switch (p->curr->tag) {
    case COMMA:
        match(p, COMMA);
        arg = funarg(p);
        funarglistp(p, fun, arg);
        break;
    case ASSIGN:
        arg->expr = assign(p);
        fundefarglistp(p, fun);
        break;
    case RPT:
        break;
    default:
        print(E, "funarglistp");
        exit(EXIT_FAILURE);
    }
}

static void funarglist(parser *p, ast_fundef *fun) {
    switch (p->curr->tag) {
    case CONST:
    case ID: {
        ast_funarg *arg = funarg(p);
        funarglistp(p, fun, arg);
        break;
    }
    case RPT:
        break;
    default:
        print(E, "funarglist");
        exit(EXIT_FAILURE);
    }
}

static ast_fundef *funsig(parser *p) {
    switch (p->curr->tag) {
    case FUN:
        match(p, FUN);
        match(p, ID);

        ast_fundef *fun = malloc(sizeof(ast_fundef));
        fun->name = strdup(p->prev->lexeme);
        fun->is_private = false;
        fun->is_native = false;
        LIST_INIT(fun, args);

        match(p, LPT);
        funarglist(p, fun);
        match(p, RPT);
        return fun;
    default:
        print(E, "funsig");
        exit(EXIT_FAILURE);
    }
}

static ast_fundef *fundef(parser *p) {
    switch (p->curr->tag) {
    case FUN: {
        ast_fundef *fun = funsig(p);
        statlist(p);
        return fun;
    }
    default:
        print(E, "fundef");
        exit(EXIT_FAILURE);
    }
}

static ast_fundef *nativefundef(parser *p) {
    switch (p->curr->tag) {
    case NATIVE:
        match(p, NATIVE);
        ast_fundef *fun = funsig(p);
        fun->is_native = true;
        return fun;
    default:
        print(E, "nativefundef");
        exit(EXIT_FAILURE);
    }
}


static ast_program_stat *privatefilep(parser *p) {
    ast_program_stat *node = malloc(sizeof(ast_stat));

    switch (p->curr->tag) {
    case CONST:
    case VAR:
        statvardecl(p);
        break;
    case NATIVE:
    case FUN:
        node->type = FUNDEF;
        node->value = (p->curr->tag == NATIVE) ? nativefundef(p) : fundef(p);
        ((ast_fundef *) node->value)->is_private = true;
        LIST_ADD(p->program->stats, node);
        return node;
    default:
        free(node);
        print(E, "privatefilep");
        exit(EXIT_FAILURE);
    }
}

static void filep(parser *p) {
    ast_program_stat *node = malloc(sizeof(ast_program_stat));

    switch (p->curr->tag) {
    case PRIVATE:
        match(p, PRIVATE);
        privatefilep(p);
        filep(p);
        break;
    case NATIVE:
    case FUN:
        node->type = FUNDEF;
        node->value = (p->curr->tag == NATIVE) ? nativefundef(p) : fundef(p);
        LIST_ADD(p->program->stats, node);
        filep(p);
        break;
    case CONST:
    case VAR:
    case ID:
        stat(p);
        filep(p);
        break;
    case EOF:
        break;
    default:
        print(E, "filep");
        exit(EXIT_FAILURE);
    }
}


static void importlist(parser *p) {
    switch (p->curr->tag) {
    case IMPORT:
        match(p, IMPORT);
        char *id = qid(p);
        LIST_ADD(p->program->imports, id);
        importlist(p);
        break;
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

static void file(parser *p) {
    switch (p->curr->tag) {
    case PRIVATE:
    case NATIVE:
    case IMPORT:
    case CONST:
    case EOF:
    case FUN:
    case VAR:
    case ID:
        importlist(p);
        filep(p);
        match(p, EOF);
        break;
    default:
        print(E, "file");
        exit(EXIT_FAILURE);
    }
}

parser *init_parser(lexer *lexer) {
    parser *p = malloc(sizeof(parser));

    p->lexer = lexer;
    p->prev = malloc(sizeof(token));
    p->curr = malloc(sizeof(token));

    p->program = malloc(sizeof(ast_program));
    LIST_INIT(p->program, imports);
    LIST_INIT(p->program, stats);

    return p;
}

ast_program *parse(parser *p) {
    move(p);
    file(p);

    ast_program *program = p->program;
    free_lexer(p->lexer);
    free(p->prev);
    free(p->curr);
    free(p);

    return program;
}