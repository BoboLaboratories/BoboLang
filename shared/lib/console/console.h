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

#ifndef CONSOLE_H
#define CONSOLE_H

#define RESET       "\e[0m"

#define RED         "\e[0;31m"
#define GREEN       "\e[0;32m"
#define BLUE        "\e[0;34m"
#define YELLOW      "\e[0;33m"

#define BOLD_RED    "\e[1;31m"
#define BOLD_GREEN  "\e[1;32m"
#define BOLD_BLUE   "\e[1;34m"
#define BOLD_YELLOW "\e[1;33m"

#define I   0,   NULL,      -1
#define E   1, __FILE__, __LINE__
#define D   2, __FILE__, __LINE__
#define W   3, __FILE__, __LINE__
#define T   4, __FILE__, __LINE__
#define N   5, __FILE__, __LINE__

void print(int mode, char *file, int line, char *format, ...);

#define DEBUG_BREAKPOINT printf("%s:%d\n", __FILE__, __LINE__)

#endif