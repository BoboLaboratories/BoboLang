#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "lib/utils.h"
#include "parser/ast.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "lang/binary/base.h"
#include "lib/console/console.h"
#include "lib/data/arraylist/arraylist.h"
#include "lib/string_utils/string_utils.h"


struct parser {
    Lexer *lexer;
    token *prev;
    token *curr;
    AST_Program *program;
};


static AST_Expr *qidexprp(Parser *p, char *qid);


static void move(Parser *p) {
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

static void match(Parser *p, int tag) {
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

#define ERR_TOO_MANY_IMPORTS    "Too many imports"
#define ERR_PROGRAM_TOO_BIG     "Program too big"
#define ERR_TOO_MANY_ARGUMENTS  "Too many arguments"

static void ast_add(ArrayList *list, void *node, char *errmsg) {
    if (!al_add(list, node)) {
        if (errno == ENOMEM) {
            print(E, "Insufficient memory in the system\n");
        } else {
            print(E, "%s\n", errmsg);
        }
        abort();
    }
}

static char *qidp(Parser *p, char *id) {
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

static char *qid(Parser *p) {
    switch (p->curr->tag) {
    case ID:
        match(p, ID);
        char *id = strdup(p->prev->lexeme);
        return qidp(p, id);
    default:
        print(E, "name");
        exit(EXIT_FAILURE);
    }
}

static AST_Expr *literal(Parser *p) {
    switch (p->curr->tag) {
    case NUM:
        match(p, NUM);
        double dbl = strtod(p->prev->lexeme, NULL);
        /* TODO strtod error checking */
        AST_Expr *node = malloc(sizeof(AST_Expr));
        node->type = EXPR_NUMERIC_LITERAL;
        node->value = malloc(sizeof(double));
        *((double *) node->value) = dbl;
        return node;
    default:
        print(E, "literal\n");
        exit(EXIT_FAILURE);
    }
}

static AST_Expr *expr(Parser *p) {
    switch (p->curr->tag) {
    case ID: {
        char *id = qid(p);
        return qidexprp(p, id);
    }
    case NUM:
        return literal(p);
    default:
        print(E, "expr");
        exit(EXIT_FAILURE);
    }
}

static void invokearglistp(Parser *p, ArrayList *args) {
    switch (p->curr->tag) {
    case COMMA:
        match(p, COMMA);
        al_add(args, expr(p));
        invokearglistp(p, args);
        break;
    case RPT:
        break;
    default:
        print(E, "invokearglistp");
        exit(EXIT_FAILURE);
    }
}

static ArrayList *invokearglist(Parser *p) {
    switch (p->curr->tag) {
    case NUM:
    case ID: {
        ArrayList *args = al_create(MAX_OF(u1));
        al_add(args, expr(p));
        invokearglistp(p, args);
        return args;
    }
    case RPT:
        return NULL;
    default:
        print(E, "invokearglist");
        exit(EXIT_FAILURE);
    }
}

static ArrayList *invokeargs(Parser *p) {
    switch (p->curr->tag) {
    case LPT:
        match(p, LPT);
        ArrayList *args = invokearglist(p);
        match(p, RPT);
        return args;
    default:
        print(E, "invokeargs");
        exit(EXIT_FAILURE);
    }
}

static AST_Expr *qidexprp(Parser *p, char *qid) {
    AST_Expr *node = malloc(sizeof(AST_Expr));

    switch (p->curr->tag) {
    case LPT:
        node->type = EXPR_INVOKE;
        node->value = malloc(sizeof(AST_ExprInvoke));
        ((AST_ExprInvoke *) node->value)->qid = qid;
        ((AST_ExprInvoke *) node->value)->args = invokeargs(p);
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
        node->value = qid;
        return node;
    default:
        print(E, "qidexprp");
        exit(EXIT_FAILURE);
    }
}

static AST_Expr *assign(Parser *p) {
    switch (p->curr->tag) {
    case ASSIGN:
        match(p, ASSIGN);
        return expr(p);
    default:
        print(E, "assign");
        exit(EXIT_FAILURE);
    }
}

static void qidstatp(Parser *p, char *qid, AST_Stat *node) {
    switch (p->curr->tag) {
    case ASSIGN:
        node->type = STAT_VAR_ASSIGN;
        node->value = malloc(sizeof(AST_StatVarAssign));
        ((AST_StatVarAssign *) node->value)->name = qid;
        ((AST_StatVarAssign *) node->value)->expr = assign(p);
        break;
    case LPT:
        node->type = STAT_INVOKE;
        node->value = malloc(sizeof(AST_StatInvoke));
        ((AST_StatInvoke *) node->value)->qid = qid;
        invokeargs(p);
        break;
    default:
        print(E, "qidstatp");
        exit(EXIT_FAILURE);
    }
}

static AST_Expr *statvardeclp(Parser *p) {
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

static AST_StatVarDecl *statvardecl(Parser *p) {
    AST_StatVarDecl *node = malloc(sizeof(AST_StatVarDecl));
    node->sig = malloc(sizeof(VariableSignature));
    node->sig->is_private = false;

    switch (p->curr->tag) {
    case CONST: {
        match(p, CONST);
        match(p, ID);
        node->sig->name = strdup(p->prev->lexeme);
        node->init = assign(p);
        node->sig->is_const = true;
        return node;
    }
    case VAR:
        match(p, VAR);
        match(p, ID);
        node->sig->name = strdup(p->prev->lexeme);
        node->init = statvardeclp(p);
        node->sig->is_const = false;
        return node;
    default:
        print(E, "statvardecl");
        exit(EXIT_FAILURE);
    }
}

static AST_Stat *stat(Parser *p) {
    AST_Stat *node = malloc(sizeof(AST_Stat));

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

static void statlistp(Parser *p, AST_FunDef *fun) {
    if (fun->stats == NULL && p->curr->tag != RPG) {
        fun->stats = al_create(MAX_OF(u4));
    }

    switch (p->curr->tag) {
    case CONST:
    case VAR:
    case ID: {
        AST_Stat *node = stat(p);
        al_add(fun->stats, node);
        statlistp(p, fun);
        break;
    }
    case RPG:
        break;
    default:
        print(E, "statlistp");
        exit(EXIT_FAILURE);
    }
}

static void statlist(Parser *p, AST_FunDef *fun) {
    switch (p->curr->tag) {
    case LPG:
        match(p, LPG);
        statlistp(p, fun);
        match(p, RPG);
        break;
    default:
        print(E, "statlist");
        exit(EXIT_FAILURE);
    }
}

static AST_FunArg *funarg(Parser *p) {
    switch (p->curr->tag) {
    case CONST:
    case ID: {
        AST_FunArg *node = malloc(sizeof(AST_FunArg));
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

static void fundefarglistp(Parser *p, AST_FunDef *fun) {
    switch (p->curr->tag) {
    case COMMA:
        match(p, COMMA);
        AST_FunArg *node = funarg(p);
        node->expr = assign(p);
        al_add(fun->sig->args, node);
        fundefarglistp(p, fun);
        break;
    case RPT:
        break;
    default:
        print(E, "fundefarglistp");
        exit(EXIT_FAILURE);
    }
}

static void funarglistp(Parser *p, AST_FunDef *fun, AST_FunArg *arg) {
    /* arg needs to be added regardless if its follows */
    al_add(fun->sig->args, arg);

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

static void funarglist(Parser *p, AST_FunDef *fun) {
    switch (p->curr->tag) {
    case CONST:
    case ID: {
        AST_FunArg *node = funarg(p);
        fun->sig->args = al_create(MAX_OF(u1));
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

static AST_FunDef *funsig(Parser *p) {
    switch (p->curr->tag) {
    case FUN:
        match(p, FUN);
        match(p, ID);

        AST_FunDef *node = malloc(sizeof(AST_FunDef));
        node->sig = malloc(sizeof(FunctionSignature));
        node->sig->name = strdup(p->prev->lexeme);
        node->sig->is_private = false;
        node->sig->is_native = false;
        node->sig->args = NULL;
        node->stats = NULL;

        match(p, LPT);
        funarglist(p, node);
        match(p, RPT);
        return node;
    default:
        print(E, "funsig");
        exit(EXIT_FAILURE);
    }
}

static AST_FunDef *fundef(Parser *p) {
    switch (p->curr->tag) {
    case FUN: {
        AST_FunDef *node = funsig(p);
        statlist(p, node);
        return node;
    }
    default:
        print(E, "fundef");
        exit(EXIT_FAILURE);
    }
}

static AST_FunDef *nativefundef(Parser *p) {
    switch (p->curr->tag) {
    case NATIVE:
        match(p, NATIVE);
        AST_FunDef *node = funsig(p);
        node->sig->is_native = true;
        return node;
    default:
        print(E, "nativefundef");
        exit(EXIT_FAILURE);
    }
}

static void privatefilep(Parser *p, AST_ProgramStat *node) {
    switch (p->curr->tag) {
    case CONST:
    case VAR:
        /*
         * private variable definitions (AST_StatVarDecl)
         * are the only root level stats that may be declared as private,
         * nevertheless they must be wrapped as a normal stat (AST_Stat)
         */
        node->type = PROGRAM_STAT;
        node->value = malloc(sizeof(AST_Stat));
        ((AST_Stat *) node->value)->type = STAT_VAR_DECL;
        ((AST_Stat *) node->value)->value = statvardecl(p);
        ((AST_StatVarDecl *) ((AST_Stat *) node->value)->value)->sig->is_private = true;
        break;
    case NATIVE:
    case FUN:
        node->type = PROGRAM_FUNDEF;
        node->value = (p->curr->tag == NATIVE) ? nativefundef(p) : fundef(p);
        ((AST_FunDef *) node->value)->sig->is_private = true;
        break;
    default:
        print(E, "privatefilep");
        exit(EXIT_FAILURE);
    }
}

static void filep(Parser *p) {
    AST_ProgramStat *node = malloc(sizeof(AST_ProgramStat));

    switch (p->curr->tag) {
    case PRIVATE:
        match(p, PRIVATE);
        privatefilep(p, node);
        ast_add(p->program->stats, node, ERR_PROGRAM_TOO_BIG);
        filep(p);
        break;
    case NATIVE:
    case FUN:
        node->type = PROGRAM_FUNDEF;
        node->value = (p->curr->tag == NATIVE) ? nativefundef(p) : fundef(p);
        ast_add(p->program->stats, node, ERR_PROGRAM_TOO_BIG);
        filep(p);
        break;
    case CONST:
    case VAR:
    case ID:
        node->type = PROGRAM_STAT;
        node->value = stat(p);
        ast_add(p->program->stats, node, ERR_PROGRAM_TOO_BIG);
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

static void importlist(Parser *p) {
    switch (p->curr->tag) {
    case IMPORT:
        match(p, IMPORT);
        char *_qid = qid(p);
        ast_add(p->program->imports, _qid, ERR_TOO_MANY_IMPORTS);
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

static void file(Parser *p) {
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

Parser *init_parser(Lexer *lexer) {
    Parser *p = malloc(sizeof(Parser));

    p->lexer = lexer;
    p->prev = malloc(sizeof(token));
    p->curr = malloc(sizeof(token));

    p->program = malloc(sizeof(AST_Program));
    p->program->imports = al_create(MAX_OF(u4));
    p->program->stats = al_create(MAX_OF(u4));

    return p;
}

AST_Program *parse(Parser *p) {
    move(p);
    file(p);

    AST_Program *program = p->program;
    free_lexer(p->lexer);
    free(p->prev);
    free(p->curr);
    free(p);

    return program;
}
