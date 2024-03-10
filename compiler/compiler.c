#include <stdio.h>

#include "lib/console.h"
#include "parser/parser.h"

static void print_expr(char *prefix, ast_expr *node) {
    if (node != NULL) {
        switch (node->type) {
        case EXPR_QID: {
            ast_expr_qid *qid = node->value;
            printf("%s%s", prefix, qid->qid);
            break;
        }

        case EXPR_INVOKE: {
            int i;
            ast_expr_invoke *invoke = node->value;
            printf("%s%s(", prefix, invoke->qid);
            if (invoke->args != NULL) {
                for (i = 0; i < list_size(invoke->args); i++) {
                    print_expr(i == 0 ? "" : ", ", list_get(invoke->args, i));
                }
            }
            printf(")");
            break;
        }
        default:
            break;
        }
    }
}

static void compile(char *path) {
    FILE *fptr = fopen(path, "r");
    Lexer *lex = init_lexer(fptr);
    parser *par = init_parser(lex);
    ast_program *prog = parse(par);

    printf("\n");

    int i, j;
    for (i = 0; i < list_size(prog->imports); i++) {
        printf("import %s\n", (char *) list_get(prog->imports, i));
    }
    printf("\n");

    for (i = 0; i < list_size(prog->stats); i++) {
        ast_program_stat *ps = list_get(prog->stats, i);
        switch (ps->type) {
        case PROGRAM_FUNDEF: {
            ast_fundef *f = ps->value;
            if (f->is_private) printf("private ");
            if (f->is_native) printf("native ");
            printf("fun %s(", f->name);
            for (j = 0; j < list_size(f->args); j++) {
                ast_funarg *a = list_get(f->args, j);
                if (a->is_const) printf("const ");
                printf("%s", a->name);
                print_expr(" = ", a->expr);
                /* if (e != NULL) {
                    printf(" = %s", ((ast_expr_qid *) e->value)->qid);
                    if (e->type == EXPR_INVOKE) {
                        int k;
                        ast_expr_invoke *in = e->value;
                        printf("(");
                        if (in->args != NULL) {
                            for (k = 0; k <= list_size(in->args); k++) {
                                ast_expr *ex = list_get(in->args, k);
                                switch (ex->type) {

                                }
                            }
                        }
                        printf(")");
                    }
                }*/
                if (j < list_size(f->args) - 1) {
                    printf(", ");
                }
            }
            printf(")");
            break;
        }
        case PROGRAM_STAT: {
            ast_stat *s = ps->value;
            switch (s->type) {
            case STAT_INVOKE:
                break;
            case STAT_VAR_DECL: {
                ast_stat_var_decl *v = s->value;
                if (v->is_private) printf("private ");
                printf(v->is_const ? "const " : "var ");
                printf("%s", v->name);
                /*if (v->init != NULL) {
                    ast_expr *e = v->init;
                    if (e != NULL) {
                        printf(" = %s", ((ast_expr_qid *) e->value)->qid);
                        if (e->type == EXPR_INVOKE) {
                            printf("(...)");
                        }
                    }
                }*/
                print_expr(" = ", v->init);
                break;
            }
            case STAT_VAR_ASSIGN:

                break;
            }
            break;
        }
        default:
            print(E, "FUCK U\n");
        }
        printf("\n\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.bobo>\n", argv[0]);
    } else {
        compile(argv[1]);
    }
}