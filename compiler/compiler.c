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
    for (i = 0; i < prog->import_list->import_count; i++) {
        printf("import %s\n", prog->import_list->imports[i]);
    }
    printf("\n");

    for (i = 0; i < prog->stat_count; i++) {
        ast_program_stat *s = prog->stats[i];
        if (s->type == FUNDEF) {
            ast_fundef *f = s->value;
            if (f->is_private) printf("private ");
            if (f->is_native) printf("native ");
            printf("fun %s(", f->name);
            for (j = 0; j < f->arg_count; j++) {
                if (f->args[j].is_const) printf("const ");
                printf("%s", f->args[j].name);
                if (j < f->arg_count - 1) {
                    printf(", ");
                }
            }
            printf(")\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.bobo>\n", argv[0]);
    } else {
        compile(argv[1]);
    }
}