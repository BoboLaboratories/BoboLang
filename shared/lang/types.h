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

#ifndef BOBO_LANG_TYPES_H
#define BOBO_LANG_TYPES_H

#include "binary/base.h"

typedef union {
    u8 bits;
    double dbl;
} Value;

# if defined(__x86_64__)
typedef u8 uptr;
# else
typedef u4 uptr;
# endif


#define QNAN        ((u8) 0x7ffc000000000000)
#define SIGN_BIT    ((u8) 0x8000000000000000)

#define TAG_NIL     1
#define TAG_FALSE   2
#define TAG_TRUE    3

#define NIL         ((Value) (u8) (QNAN | TAG_NIL))
#define FALSE       ((Value) (u8) (QNAN | TAG_FALSE))
#define TRUE        ((Value) (u8) (QNAN | TAG_TRUE))
#define WRAP_NUM(n) ((Value) (double) (n))
#define WRAP_PTR(p) ((Value) (SIGN_BIT | QNAN | (uptr) (p)))

#define IS_NIL(v)   ((v.bits) == (NIL.bits))
#define IS_BOOL(v)  ((v.bits) == (TRUE.bits) || (v.bits) == (FALSE.bits))
#define IS_NUM(v)   (((v.bits) & QNAN) != QNAN)
#define IS_PTR(v)   (((v.bits) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define AS_BOOL(v)  ((v.bits) == (TRUE.bits))
#define AS_NUM(v)   (v.dbl)
#define AS_PTR(v)   ((void *) (uptr) ((v.bits) & ~(SIGN_BIT | QNAN)))

#endif