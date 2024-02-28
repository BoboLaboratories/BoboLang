#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "lib/console.h"

static const char *prefixes[] = {
        RESET       " INFO " RESET,
        BOLD_RED    "ERROR " RED,
        BOLD_BLUE   "DEBUG " BLUE,
        BOLD_YELLOW " WARN " YELLOW,
        RESET       "      " RESET,
        RESET       ""       RESET
};

void print(int mode, char *file, int line, char *format, ...) {
    int errno_bak = errno;

    FILE *fd = stdout;
    if (mode == 1) {
        fd = stderr;
    }

    fprintf(fd, "%s", prefixes[mode]);

    if (mode == 1 && errno != 0) {
        fprintf(fd, "errno %d: %s (%s:%d).\n", errno_bak, strerror(errno_bak), file, line);
    }

    va_list args;
    va_start(args, format);
    vfprintf(fd, format, args);
    fprintf(fd, RESET);
    va_end(args);

    errno = errno_bak;
}