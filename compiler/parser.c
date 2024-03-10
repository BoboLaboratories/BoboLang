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


static ast_expr *qidexprp(parser *p, char *qid);


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
        size_t size = strlen(id) + 1 /* for . */ + strlen(p->prev->lexeme) + 1 /* for \0 */;
        char *tmp = realloc(id, size);
        if (tmp == NULL) {
            print(E, "Could not allocate more memory\n");
            exit(EXIT_FAILURE);
        }
        id = tmp;
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

static ast_expr *expr(parser *p) {
    switch (p->curr->tag) {
    case ID: {
        char *id = qid(p);
        return qidexprp(p, id);
    }
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

static ast_expr *qidexprp(parser *p, char *qid) {
    ast_expr *node = malloc(sizeof(ast_expr));

    switch (p->curr->tag) {
    case LPT:
        node->type = EXPR_INVOKE;
        node->value = malloc(sizeof(ast_expr_invoke));
        ((ast_expr_invoke *) node->value)->qid = qid;
        invokeargs(p);
        return node;
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
        node->type = EXPR_QID;
        node->value = malloc(sizeof(ast_expr_qid));
        ((ast_expr_qid *) node->value)->qid = qid;
        return node;
    default:
        print(E, "qidexprp");
        exit(EXIT_FAILURE);
    }
}

static ast_expr *assign(parser *p) {
    switch (p->curr->tag) {
    case ASSIGN:
        match(p, ASSIGN);
        return expr(p);
    default:
        print(E, "assign");
        exit(EXIT_FAILURE);
    }
}

static void qidstatp(parser *p, char *qid, ast_stat *node) {
    switch (p->curr->tag) {
    case ASSIGN:
        node->type = STAT_VAR_ASSIGN;
        node->value = malloc(sizeof(ast_stat_var_assign));
        ((ast_stat_var_assign *) node->value)->qid = qid;
        ((ast_stat_var_assign *) node->value)->expr = assign(p);
        break;
    case LPT:
        node->type = STAT_INVOKE;
        node->value = malloc(sizeof(ast_stat_invoke));
        ((ast_stat_invoke *) node->value)->qid = qid;
        invokeargs(p);
        break;
    default:
        print(E, "qidstatp");
        exit(EXIT_FAILURE);
    }
}

static ast_expr *statvardeclp(parser *p) {
    switch (p->curr->tag) {
    case ASSIGN:
        return assign(p);
    case PRIVATE:
    case NATIVE:
    case CONST:
    case EOF:
    case FUN:
    case RPG:
    case VAR:
    case ID:
        return NULL;
    default:
        print(E, "statvardeclp");
        exit(EXIT_FAILURE);
    }
}

static ast_stat_var_decl *statvardecl(parser *p) {
    ast_stat_var_decl *node = malloc(sizeof(ast_stat_var_decl));
    node->is_private = false;

    switch (p->curr->tag) {
    case CONST: {
        match(p, CONST);
        match(p, ID);
        node->name = strdup(p->prev->lexeme);
        node->init = assign(p);
        node->is_const = true;
        return node;
    }
    case VAR:
        match(p, VAR);
        match(p, ID);
        node->name = strdup(p->prev->lexeme);
        node->init = statvardeclp(p);
        node->is_const = false;
        return node;
    default:
        print(E, "statvardecl");
        exit(EXIT_FAILURE);
    }
}

static ast_stat *stat(parser *p) {
    ast_stat *node = malloc(sizeof(ast_stat));

    switch (p->curr->tag) {
    case CONST:
    case VAR:
        node->type = STAT_VAR_DECL;
        node->value = statvardecl(p);
        return node;
    case ID: {
        char *_qid = qid(p);
        qidstatp(p, _qid, node);
        return node;
    }
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
        ast_funarg *node = malloc(sizeof(ast_funarg));
        node->is_const = p->curr->tag == CONST;
        if (node->is_const) {
            match(p, CONST);
        }
        match(p, ID);
        node->name = strdup(p->prev->lexeme);
        node->expr = NULL;
        return node;
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
        ast_funarg *node = funarg(p);
        node->expr = assign(p);
        LIST_ADD(fun->args, node);
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
    /* arg needs to be added regardless if its follows */
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
        ast_funarg *node = funarg(p);
        funarglistp(p, fun, node);
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

        ast_fundef *node = malloc(sizeof(ast_fundef));
        node->name = strdup(p->prev->lexeme);
        node->is_private = false;
        node->is_native = false;
        LIST_INIT(node, args);

        match(p, LPT);
        funarglist(p, node);
        match(p, RPT);
        return node;
    default:
        print(E, "funsig");
        exit(EXIT_FAILURE);
    }
}

static ast_fundef *fundef(parser *p) {
    switch (p->curr->tag) {
    case FUN: {
        ast_fundef *node = funsig(p);
        statlist(p);
        return node;
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
        ast_fundef *node = funsig(p);
        node->is_native = true;
        return node;
    default:
        print(E, "nativefundef");
        exit(EXIT_FAILURE);
    }
}

static void privatefilep(parser *p, ast_program_stat *node) {
    switch (p->curr->tag) {
    case CONST:
    case VAR:
        /*
         * private variable definitions (ast_stat_var_decl)
         * are the only root level stats that may be declared as private,
         * nevertheless they must be wrapped as a normal stat (ast_stat)
         */
        node->type = PROGRAM_STAT;
        node->value = malloc(sizeof(ast_stat));
        ((ast_stat *) node->value)->type = STAT_VAR_DECL;
        ((ast_stat *) node->value)->value = statvardecl(p);
        ((ast_stat_var_decl *) ((ast_stat *) node->value)->value)->is_private = true;
        break;
    case NATIVE:
    case FUN:
        node->type = PROGRAM_FUNDEF;
        node->value = (p->curr->tag == NATIVE) ? nativefundef(p) : fundef(p);
        ((ast_fundef *) node->value)->is_private = true;
        break;
    default:
        print(E, "privatefilep");
        exit(EXIT_FAILURE);
    }
}

static void filep(parser *p) {
    ast_program_stat *node = malloc(sizeof(ast_program_stat));

    switch (p->curr->tag) {
    case PRIVATE:
        match(p, PRIVATE);
        privatefilep(p, node);
        LIST_ADD(p->program->stats, node);
        filep(p);
        break;
    case NATIVE:
    case FUN:
        node->type = PROGRAM_FUNDEF;
        node->value = (p->curr->tag == NATIVE) ? nativefundef(p) : fundef(p);
        LIST_ADD(p->program->stats, node);
        filep(p);
        break;
    case CONST:
    case VAR:
    case ID:
        node->type = PROGRAM_STAT;
        node->value = stat(p);
        LIST_ADD(p->program->stats, node);
        filep(p);
        break;
    case EOF:
        free(node);
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
