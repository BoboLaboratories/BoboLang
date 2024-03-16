/*
 * MIT License
 *
 * Copyright (C) 2024 BoboLabs.net
 * Copyright (C) 2024 Mattia Mignogna (https://stami.bobolabs.net)
 * Copyright (C) 2024 Fabio Nebbia (https://glowy.bobolabs.net)
 * Copyright (C) 2024 Third party contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "console.h"

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