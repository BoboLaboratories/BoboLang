#include <stdio.h>

#include "lib/console.h"
#include "translator.h"

static void compile(char *path) {
    FILE *fptr = fopen(path, "r");
    lexer lexer = init_lexer(fptr);
    translator translator = init_translator(&lexer);
    bobo_bin *bin = translate(&translator);

    int i;
    for (i = 0; i < bin->import_count; i++) {
        print(D, "%s\n", bin->imports[i]);
    }

    fclose(fptr);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.bobo>\n", argv[0]);
    } else {
        compile(argv[1]);
    }
}