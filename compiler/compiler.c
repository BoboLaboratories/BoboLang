#include <stdio.h>


#include "token.h"
#include "lexer.h"
#include "lib/console.h"

#include "translator.h"

int main(int argc, char *argv[]) {
    FILE *fptr = fopen(argv[1], "r");

    translate(fptr);

/*
    token tok;
    do {
        tok = scan(fptr);
        TOK_PRINT(tok);
    } while(tok.tag != EOF);
*/

    fclose(fptr);
}