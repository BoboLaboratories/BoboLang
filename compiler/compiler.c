#include <stdio.h>

#include "lexer.h"
#include "lib/console.h"

int main(int argc, char *argv[]) {
    FILE *fptr = fopen(argv[1], "r");
    token tok;
    do {
        tok = scan(fptr);
        TOK_PRINT(tok);
    } while (tok.tag != EOF && tok.tag != ERR);
    fclose(fptr);
}