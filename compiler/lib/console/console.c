#include <stdio.h>
#include <stdarg.h>

#include "lexer/token.h"
#include "lib/console/console.h"

#define TAB "    "

void error(Meta *meta, unsigned long line, char *fmt, ...) {
    fprintf(stderr, "[%s:%ld] error: ", meta->pathname, line);

    va_list args;
    va_start(args, fmt);
    fprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, TAB);
    char *curr = al_get(meta->line_info, line - 1);

    while (*curr != '\n' && *curr != EOP) {
        fprintf(stderr, "%c", *curr);
        curr++;
    }
    fprintf(stderr, "\n");
}