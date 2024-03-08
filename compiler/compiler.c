#include <stdio.h>

#include "lib/console.h"
#include "parser/parser.h"

static void compile(char *path) {
    FILE *fptr = fopen(path, "r");
    lexer *lex = init_lexer(fptr);
    parser *par = init_parser(lex);
    ast_program *prog = parse(par);

    printf("\n");

    int i, j;
    for (i = 0; i < prog->imports->count; i++) {
        printf("import %s\n", prog->imports->values[i]);
    }
    printf("\n");

    for (i = 0; i < prog->stats->count; i++) {
        ast_program_stat *ps = prog->stats->values[i];
        switch (ps->type) {
        case PROGRAM_FUNDEF: {
            ast_fundef *f = ps->value;
            if (f->is_private) printf("private ");
            if (f->is_native) printf("native ");
            printf("fun %s(", f->name);
            for (j = 0; j < f->args->count; j++) {
                if (f->args->values[j]->is_const) printf("const ");
                printf("%s", f->args->values[j]->name);
                ast_expr *e = f->args->values[j]->expr;
                if (e != NULL) {
                    printf(" = %s", ((ast_expr_qid *) e->value)->qid);
                    if (e->type == EXPR_INVOKE) {
                        printf("(...)");
                    }
                }
                if (j < f->args->count - 1) {
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
                if (v->init != NULL) {
                    ast_expr *e = v->init;
                    if (e != NULL) {
                        printf(" = %s", ((ast_expr_qid *) e->value)->qid);
                        if (e->type == EXPR_INVOKE) {
                            printf("(...)");
                        }
                    }
                }
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