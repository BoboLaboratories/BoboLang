#include <ctype.h>
#include <stdlib.h>

#include "common.h"
#include "lexer/lexer.h"
#include "numeric_literal.h"

static token *accept(Lexer *lexer) {
    return mktokl(NUM, get_buffer(lexer));
}

static void reject(Lexer *lexer) {
    lexer_err(lexer, "bad number format\n");
}

token *scan_numeric_literal(Lexer *lexer, NumericLiteralState state) {
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
                state = INTEGER;
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
        case INTEGER:
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
                state = INTEGER;
            } else if (is_whitespace(c)) {
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
                state = DECIMAL;
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
                state = DECIMAL;
            } else {
                reject(lexer);
            }
            break;
            /*
             * we read "n.m", "+n.m" or "-n.m"
             */
        case DECIMAL:
            if (isdigit(c)) {
                /*
                 * we read one more digit therefore
                 * we continue lexing the decimal part
                 */
                state = DECIMAL;
            } else if (tolower(c) == 'e') {
                /*
                 * we read 'e' or 'E' so we need to read the exponent
                 */
                state = EXPONENT_SIGN;
            } else if (is_whitespace(c)) {
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
                state = EXPONENT;
            } else {
                reject(lexer);
            }
            break;
            /*
             * we read "nek", "+nek", "-nek", "n.mek", "+n.mek" or "-n.mek"
             */
        case EXPONENT:
            if (isdigit(c)) {
                /*
                 * we read one more digit therefore
                 * we continue lexing the exponent
                 */
                state = EXPONENT;
            } else if (is_whitespace(c)) {
                return accept(lexer);
            } else {
                lexer->column++;
                reject(lexer);
            }
            break;
        }
    }
}