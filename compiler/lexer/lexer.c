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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "common.h"
#include "numeric_literal.h"
#include "lib/console/console.h"

#define LEXER_RESET '\0'

static void single_line_comment(Lexer *lexer);

static void multi_line_comment(Lexer *lexer);

static token *scan_id(Lexer *lexer);

static int is_new_line(char c);

Lexer *init_lexer(Meta *meta) {
    Lexer *lexer = malloc(sizeof(Lexer));

    lexer->fptr = fopen(meta->pathname, "r");
    lexer->meta = meta;
    lexer->buf_start = -1;
    lexer->line = 1;
    lexer->column = 1;
    reset(lexer);

    return lexer;
}

void reset(Lexer *lexer) {
    lexer->peek = LEXER_RESET;
}

char next(Lexer *lexer) {
    if (lexer->peek != LEXER_RESET) {
        lexer->column++;
    }

    if (lexer->peek == '\n') {
        lexer->line_start = ftell(lexer->fptr);
        lexer->column = 1;
        lexer->line++;
    }

    lexer->peek = (char) fgetc(lexer->fptr);

    return lexer->peek;
}

token *scan(Lexer *lexer) {
    if (lexer->peek == LEXER_RESET) {
        next(lexer);
    }

    while (is_whitespace(lexer->peek)) {
        next(lexer);
    }

    switch (lexer->peek) {
    case '(':
        return mktokr(TOK_LPT);
    case ')':
        return mktokr(TOK_RPT);
    case '{':
        return mktokr(TOK_LPG);
    case '}':
        return mktokr(TOK_RPG);
    case '.': {
        token *numeric_literal = scan_numeric_literal(lexer, IMPLICIT_INTEGER_PART);
        return numeric_literal != NULL ? numeric_literal : mktok(TOK_DOT);
    }
    case '*':
        return mktokr(TOK_MUL);
    case '+': {
        token *numeric_literal = scan_numeric_literal(lexer, INTEGER_SIGN);
        return numeric_literal != NULL ? numeric_literal : mktok(TOK_PLUS);
    }
    case '/':
        switch (next(lexer)) {
        case '/':
            single_line_comment(lexer);
            return scan(lexer);
        case '*':
            multi_line_comment(lexer);
            return scan(lexer);
        default:
            return mktok(TOK_DIV);
        }
    case ',':
        return mktokr(TOK_COMMA);
    case '<':
        return (next(lexer) == '=') ? mktokr(TOK_LE) : mktok(TOK_LT);
    case '>':
        return (next(lexer) == '=') ? mktokr(TOK_GE) : mktok(TOK_GT);
    case '!':
        return (next(lexer) == '=') ? mktokr(TOK_NEQ) : mktok(TOK_NOT);
    case '=':
        return (next(lexer) == '=') ? mktokr(TOK_EQ) : mktok(TOK_ASSIGN);
    case '-': {
        token *numeric_literal = scan_numeric_literal(lexer, INTEGER_SIGN);
        if (numeric_literal != NULL) {
            return numeric_literal;
        } else {
            return (next(lexer) == '>') ? mktokr(TOK_ARROW) : mktok(TOK_MINUS);
        }
    }
    case '&':
        if (next(lexer) != '&') {
            print(E, "illegal symbol &%c at line %d\n", lexer->peek, lexer->line);
            exit(EXIT_FAILURE);
        }
        return mktokr(TOK_AND);
    case '|':
        if (next(lexer) != '|') {
            print(E, "illegal symbol |%c at line %d\n", lexer->peek, lexer->line);
            exit(EXIT_FAILURE);
        }
        return mktokr(TOK_OR);
    case EOF:
        return mktokl(EOF, NULL);
    default:
        if (isalpha(lexer->peek) || lexer->peek == '_') {
            return scan_id(lexer);
        } else if (isdigit(lexer->peek)) {
            token *tok = scan_numeric_literal(lexer, INTEGER_PART);
            if (tok != NULL) {
                reset(lexer);
                return tok;
            }
        }

        print(E, "erroneous symbol %c at line %d\n", lexer->peek, lexer->line);
        exit(EXIT_FAILURE);
    }
}

void start_buffering(Lexer *lexer) {
    lexer->buf_start = ftell(lexer->fptr) - 1;
}

char *get_buffer(Lexer *lexer) {
    if (lexer->buf_start == -1) {
        print(E, "Never started buffering\n");
        abort();
    }

    long end = ftell(lexer->fptr);
    if (lexer->peek != EOF) {
        end--;
    }
    long len = end - lexer->buf_start;
    char *lexeme = malloc(len + 1);

    fseek(lexer->fptr, lexer->buf_start, SEEK_SET);
    fread(lexeme, len, 1, lexer->fptr);
    *(lexeme + len) = '\0';

    lexer->buf_start = -1;

    return lexeme;
}

void free_lexer(Lexer *lexer) {
    fclose(lexer->fptr);
    free(lexer);
}

static void single_line_comment(Lexer *lexer) {
    do {
        next(lexer);
    } while (lexer->peek != '\n' && lexer->peek != EOF);
}

static void multi_line_comment(Lexer *lexer) {
    unsigned int line = lexer->line;
    int state = 0;

    do {
        next(lexer);
        if (state == 0) {
            if (lexer->peek == '*') {
                state = 1;
            }
        } else {
            if (lexer->peek == '/') {
                state = 2;
            } else if (lexer->peek != '*') {
                state = 0;
            }
        }
    } while (state != 2 && lexer->peek != EOF);

    if (state == 2) {
        lexer->peek = ' ';
    } else {
        print(E, "unclosed multi line comment starting at line %d\n", line);
        exit(EXIT_FAILURE);
    }
}

static token *scan_id(Lexer *lexer) {
    start_buffering(lexer);

    int underscore_only = lexer->peek == '_';
    do {
        underscore_only &= lexer->peek == '_';
        next(lexer);
    } while (isalnum(lexer->peek) || lexer->peek == '_');

    char *lexeme = get_buffer(lexer);

    if (underscore_only) {
        print(E, "illegal symbol %s at line %d\n", lexeme, lexer->line);
        exit(EXIT_FAILURE);
    } else {
        token *ret;
        if (strcmp(lexeme, "import") == 0) {
            ret = mktokr(TOK_IMPORT);
        } else if (strcmp(lexeme, "fun") == 0) {
            ret = mktokr(TOK_FUN);
        } else if (strcmp(lexeme, "private") == 0) {
            ret = mktokr(TOK_PRIVATE);
        } else if (strcmp(lexeme, "native") == 0) {
            ret = mktokr(TOK_NATIVE);
        } else if (strcmp(lexeme, "const") == 0) {
            ret = mktokr(TOK_CONST);
        } else if (strcmp(lexeme, "var") == 0) {
            ret = mktokr(TOK_VAR);
        } else if (strcmp(lexeme, "check") == 0) {
            ret = mktokr(TOK_CHECK);
        } else {
            ret = mktoklr(ID, lexeme);
        }

        if (ret->tag != ID) {
            free(lexeme);
        }

        return ret;
    }
}

int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static int is_new_line(char c) {
    return c == '\n' || c == '\r';
}

token *make_token(Lexer *lexer, int tag, char *lexeme, bool is_reset) {
    if (is_reset) {
        reset(lexer);
    }

    token *tok = malloc(sizeof(token));
    tok->line = lexer->line;
    tok->lexeme = lexeme;
    tok->tag = tag;

    return tok;
}

void lexer_err(Lexer *lexer, char *msg) {
    unsigned int i;
    unsigned int err_column = lexer->column;
    if (err_column > 0) {
        err_column--;
    }

    printf("[%s:%d] error: %s", lexer->meta->pathname, lexer->line, msg);
    printf("\t");
    fseek(lexer->fptr, lexer->line_start, SEEK_SET);

    while (1) {
        lexer->peek = (char) fgetc(lexer->fptr);
        if (!is_new_line(lexer->peek) && lexer->peek != EOF) {
            printf("%c", lexer->peek);
        } else {
            break;
        }
    }

    printf("\n\t");
    for (i = 0; i < err_column - 1; i++) {
        printf(" ");
    }
    printf("^\n");

    exit(EXIT_FAILURE);
}