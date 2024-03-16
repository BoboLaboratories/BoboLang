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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "lang/types.h"

#if defined(__GNUC__)
#define USE_LABELS
#endif

#ifdef USE_LABELS
#define CASE(opcode)    opcode
#define DISPATCH()      goto *(dispatch_table[*pc++])
#else
#define CASE(opcode)    case opcode
#define DISPATCH()      goto run
#endif

enum bytecode {
    LDC = 1,
    END = 2,
    ADD = 3,
    SUB = 4,
    MUL = 5,
    DIV = 6
};

#define DEBUG_STACK(note)   {                                                   \
                                printf("%16s: ", note);                         \
                                int *p;                                         \
                                for (p = sp - 1; p >= (int *) &stack; p--) {    \
                                    printf("%d ", *p);                          \
                                }                                               \
                                printf("\n");                                   \
                            }

#define POP(type)         *((type *) (--sp))
#define PUSH(type, value) (*((type *) (sp++)) = (value))

typedef long word;

word stack[10];
word *sp = (word *) &stack;

static void bobo(unsigned char *code) {
    unsigned char *pc = code;

#ifdef USE_LABELS
    void *dispatch_table[] = {
            [LDC] = &&ldc,          [DIV] = &&div,          [END] = &&end
    };
#endif

#ifndef USE_LABELS
    switch(*pc) {
#else
    DISPATCH();
#endif

    CASE(ldc):
    {
        PUSH(int, *pc++);
        DISPATCH();
    }

    CASE(div):
    {
        double c = POP(double);
        double d = POP(double);
        PUSH(double, d / c);
        DISPATCH();
    }

    CASE(end):
    {
        printf("%f\n", POP(float));
        exit(0);
    }

#ifndef USE_LABELS
    }
#endif
}

int main() {
    /*
    FILE* fptr = fopen("bin.bobo", "r");
    fread(code, 1, sizeof(void *) * 2, fptr);
    fclose(fptr);
    */

    unsigned char program[] = {
            LDC, 5, LDC, 2, DIV, END
    };

    bobo(program);
}
