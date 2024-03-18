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

#ifndef BOBO_LANG_COMPILER_LEXER_COMMON_H
#define BOBO_LANG_COMPILER_LEXER_COMMON_H

#include <stdbool.h>

#include "meta.h"
#include "lexer/token.h"
#include "lexer/lexer.h"
#include "lib/console/console.h"

struct lexer {
    Meta *meta;
    char *buf;
    char *peek;
    unsigned long line;
    unsigned long column;
};

/* make Token with well known lexeme */
#define mktok(tok)              make_token(lexer, tok, false)

/* make Token with well known lexeme and reset peek */
#define mktokr(tok)             make_token(lexer, tok, true)

/* make Token with the given tag and lexeme */
#define mktokl(tag, lexeme)     make_token(lexer, tag, lexeme, false)

/* make Token with the given tag and lexeme and reset peek */
#define mktoklr(tag, lexeme)    make_token(lexer, tag, lexeme, true)

void reset(Lexer *lexer);

char next(Lexer * lexer);

Token *make_token(Lexer *lexer, int tag, char *lexeme, bool reset);

void start_buffering(Lexer *lexer);

char *get_buffer(Lexer *lexer);

int is_whitespace(char c);

void lexer_err(Lexer *lexer, char *msg);

#endif