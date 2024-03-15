#include <string.h>
#include <malloc.h>

#include "meta.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "translator/translator.h"

struct translator {
    Meta *meta;
    AST_Program *program;
};

Translator *init_translator(Meta *meta) {
    Translator *translator = malloc(sizeof(Translator));

    translator->meta = meta;
    translator->program = NULL;

    return translator;
}

static void disassemble(AST_Program *program);

BinaryModule *translate(Translator *translator) {
    Lexer *lexer = init_lexer(translator->meta);
    Parser *parser = init_parser(lexer);
    AST_Program *program = parse(parser);
    disassemble(program);
    return NULL;
}


static void print_expr(char *prefix, AST_Expr *node) {
    if (node != NULL) {
        switch (node->type) {
        case EXPR_QID: {
            printf("%s%s", prefix, (char *) node->value);
            break;
        }
        case EXPR_INVOKE: {
            int i;
            AST_ExprInvoke *invoke = node->value;
            printf("%s%s(", prefix, invoke->qid);
            if (invoke->args != NULL) {
                for (i = 0; i < al_size(invoke->args); i++) {
                    print_expr(i == 0 ? "" : ", ", al_get(invoke->args, i));
                }
            }
            printf(")");
            break;
        }
        case EXPR_NUMERIC_LITERAL: {
            printf("%s%g", prefix, *((double *) node->value));
        }
        default:
            break;
        }
    }
}

static void print_stat(char *prefix, AST_Stat *node) {
    if (node != NULL) {
        switch (node->type) {
        case STAT_INVOKE: {
            AST_StatInvoke *v = node->value;
            printf("%s%s(", prefix, v->qid);
            if (v->args != NULL) {
                int i;
                for (i = 0; i < al_size(v->args); i++) {
                    print_expr(i == 0 ? "" : ", ", al_get(v->args, i));
                }
            }
            printf(")");
            break;
        }
        case STAT_VAR_DECL: {
            AST_StatVarDecl *v = node->value;
            printf("%s", prefix);
            if (v->is_private) printf("private ");
            printf(v->is_const ? "const " : "var ");
            printf("%s", v->name);
            print_expr(" = ", v->init);
            break;
        }
        case STAT_VAR_ASSIGN: {
            AST_StatVarAssign *v = node->value;
            printf("%s%s", prefix, v->qid);
            print_expr(" = ", v->expr);
            break;
        }
        }
    }
    printf("\n");
}

static void disassemble(AST_Program *program) {
    printf("\n");

    int i, j;
    for (i = 0; i < al_size(program->imports); i++) {
        printf("import %s\n", (char *) al_get(program->imports, i));
    }
    printf("\n");

    for (i = 0; i < al_size(program->stats); i++) {
        AST_ProgramStat *ps = al_get(program->stats, i);
        switch (ps->type) {
        case PROGRAM_FUNDEF: {
            AST_FunDef *f = ps->value;
            if (f->is_private) printf("private ");
            if (f->is_native) printf("native ");
            printf("fun %s(", f->name);
            if (f->args != NULL) {
                for (j = 0; j < al_size(f->args); j++) {
                    AST_FunArg *a = al_get(f->args, j);
                    if (a->is_const) printf("const ");
                    printf("%s", a->name);
                    print_expr(" = ", a->expr);
                    if (j < al_size(f->args) - 1) {
                        printf(", ");
                    }
                }
            }
            printf(")");
            if (!f->is_native) printf(" {\n");
            if (f->stats != NULL) {
                for (j = 0; j < al_size(f->stats); j++) {
                    print_stat("\t", al_get(f->stats, j));
                }
            }
            if (!f->is_native) printf("}");
            break;
        }
        case PROGRAM_STAT: {
            print_stat("", ps->value);
            break;
        }
        default:
            printf("FUCK U\n");
        }
        printf("\n");
    }
}
