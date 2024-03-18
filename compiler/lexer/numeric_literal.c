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

#include <ctype.h>
#include <stdlib.h>

#include "common.h"
#include "lexer/lexer.h"
#include "numeric_literal.h"

static Token *accept(Lexer *lexer) {
    return mktokl(NUM, get_buffer(lexer));
}

static void reject(Lexer *lexer) {
    error(lexer->meta, lexer->line, "bad number format\n");
    exit(EXIT_FAILURE);
}

static bool can_terminate(char c) {
    return is_whitespace(c)
           || c == EOF
           || c == COMMA
           || c == RPT
           || c == RPG;
}

Token *scan_numeric_literal(Lexer *lexer, NumericLiteralState state) {
    start_buffering(lexer);

    while (1) {
        const char c = next(lexer);
        switch (state) {
            /*
             * we read '+' or '-'
             */
        case INTEGER_SIGN:
            if (c == '.') {
                /*
                 * we read '.' which could mean we have
                 * a signed numeric literal with
                 * implicit integer part 0
                 */
                state = IMPLICIT_INTEGER_PART;
            } else if (isdigit(c)) {
                /*
                 * we read 'n', where n is a digit therefore
                 * we surely have a valid signed numeric literal
                 */
                state = INTEGER_PART;
            } else {
                /*
                 * after reading '+' or '-' we found nothing
                 * that could be lexed as a numeric literal
                 * therefore we return and let the lexer
                 * do its defaults for such symbols
                 */
                return NULL;
            }
            /*
             * we read "n", "+n" or "-n" where n
             * is made up of one or more digits
             */
        case INTEGER_PART:
            if (c == '.') {
                /*
                 * read '.' so we need to read the decimal part
                 */
                state = INCOMPLETE_DECIMAL;
            } else if (tolower(c) == 'e') {
                /*
                 * we read 'e' or 'E' so we need to read the exponent
                 */
                state = EXPONENT_SIGN;
            } else if (isdigit(c)) {
                /*
                 * we read one more digit therefore
                 * we continue lexing the integer part
                 */
                state = INTEGER_PART;
            } else if (can_terminate(c)) {
                return accept(lexer);
            } else {
                lexer->column++;
                reject(lexer);
            }
            break;
            /*
             * we read '.'
             */
        case IMPLICIT_INTEGER_PART:
            if (isdigit(c)) {
                /*
                 * we read a digit therefore we know
                 * this is now a decimal number "0.n"
                 */
                state = DECIMAL_PART;
            } else {
                /*
                 * after reading '.' we found nothing that
                 * could be lexed as a numeric literal
                 * therefore we return and let the lexer
                 * do its defaults for such symbols
                 */
                return NULL;
            }
            break;
            /*
             * we read "n.", "+n." or "-n."
             */
        case INCOMPLETE_DECIMAL:
            if (isdigit(c)) {
                /*
                 * we read a digit therefore we know
                 * this is now a valid decimal number
                 */
                state = DECIMAL_PART;
            } else {
                reject(lexer);
            }
            break;
            /*
             * we read "n.m", "+n.m" or "-n.m"
             */
        case DECIMAL_PART:
            if (isdigit(c)) {
                /*
                 * we read one more digit therefore
                 * we continue lexing the decimal part
                 */
                state = DECIMAL_PART;
            } else if (tolower(c) == 'e') {
                /*
                 * we read 'e' or 'E' so we need to read the exponent
                 */
                state = EXPONENT_SIGN;
            } else if (can_terminate(c)) {
                return accept(lexer);
            } else {
                lexer->column++;
                reject(lexer);
            }
            break;
            /*
             * we read "ne", "+ne", "-ne", "n.me", "+n.me" or "-n.me"
             */
        case EXPONENT_SIGN:
            if (c == '+' || c == '-' || isdigit(c)) {
                /*
                 * we read '+', '-' or one more digit
                 * (the latter implies a positive exponent)
                 * therefore we continue lexing the exponent
                 */
                state = EXPONENT_PART;
            } else {
                reject(lexer);
            }
            break;
            /*
             * we read "nek", "+nek", "-nek", "n.mek", "+n.mek" or "-n.mek"
             */
        case EXPONENT_PART:
            if (isdigit(c)) {
                /*
                 * we read one more digit therefore
                 * we continue lexing the exponent
                 */
                state = EXPONENT_PART;
            } else if (can_terminate(c)) {
                return accept(lexer);
            } else {
                lexer->column++;
                reject(lexer);
            }
            break;
        }
    }
}