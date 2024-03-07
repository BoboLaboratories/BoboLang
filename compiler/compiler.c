#include <stdio.h>

#include "translator.h"
#include "lib/console.h"
#include "disassembler.h"

static void compile(char *path) {
    FILE *fptr = fopen(path, "r");
    lexer *lex = init_lexer(fptr);
    translator *tr = init_translator(lex);
    bobo_bin *bin = translate(tr);

    int i, j;
    for (i = 0; i < bin->function_count; i++) {
        function *fun = bin->functions[i];
        if (CHECK_BIT(fun->mod, FUN_MOD_PRIVATE)) printf("private ");
        if (CHECK_BIT(fun->mod, FUN_MOD_NATIVE)) printf("native ");
        printf("fun %s(", fun->name);
        for (j = 0; j < fun->max_args; j++) {
            printf("$%d", j);
            if (j >= fun->min_args) {
                printf(" = <?>");
            }
            if (j < fun->max_args - 1) {
                printf(", ");
            }
        }
        printf(")\n");
    }

    disassemble(bin);
    fclose(fptr);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.bobo>\n", argv[0]);
    } else {
        compile(argv[1]);
    }
}