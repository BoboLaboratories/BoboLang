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
#include <limits.h>

#include "common.h"
#include "numeric_literal.h"

static void single_line_comment(Lexer *lexer);

static void multi_line_comment(Lexer *lexer);

static Token *scan_id(Lexer *lexer);

static int is_new_line(char c);

Lexer *init_lexer(Meta *meta) {
    Lexer *lexer = malloc(sizeof(Lexer));

    lexer->meta = meta;
    lexer->line = lexer->column = 1;
    lexer->peek = lexer->buf = (char *) meta->code;

    meta->line_info = al_create(ULONG_MAX);
    al_add(meta->line_info, lexer->peek);

    return lexer;
}

void reset(Lexer *lexer) {
    lexer->peek++;
}

char next(Lexer *lexer) {
    if (*lexer->peek == '\n') {
        al_add(lexer->meta->line_info, lexer->peek + 1);
        lexer->column = 1;
        lexer->line++;
    } else {
        lexer->column++;
    }

    lexer->peek++;

    return *lexer->peek;
}

Token *scan(Lexer *lexer) {
    while (is_whitespace(*lexer->peek)) {
        next(lexer);
    }

    switch (*lexer->peek) {
    case '(':
        return mktokr(TOK_LPT);
    case ')':
        return mktokr(TOK_RPT);
    case '{':
        return mktokr(TOK_LPG);
    case '}':
        return mktokr(TOK_RPG);
    case '.': {
        Token *numeric_literal = scan_numeric_literal(lexer, IMPLICIT_INTEGER_PART);
        return numeric_literal != NULL ? numeric_literal : mktok(TOK_DOT);
    }
    case '*':
        return mktokr(TOK_MUL);
    case '+': {
        Token *numeric_literal = scan_numeric_literal(lexer, INTEGER_SIGN);
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
        Token *numeric_literal = scan_numeric_literal(lexer, INTEGER_SIGN);
        if (numeric_literal != NULL) {
            return numeric_literal;
        } else {
            return (next(lexer) == '>') ? mktokr(TOK_ARROW) : mktok(TOK_MINUS);
        }
    }
    case '&':
        if (next(lexer) != '&') {
            error(lexer->meta, lexer->line, "illegal symbol &%c\n", *lexer->peek);
            exit(EXIT_FAILURE);
        }
        return mktokr(TOK_AND);
    case '|':
        if (next(lexer) != '|') {
            error(lexer->meta, lexer->line, "illegal symbol |%c\n", *lexer->peek);
            exit(EXIT_FAILURE);
        }
        return mktokr(TOK_OR);
    case EOP:
        return mktokl(EOP, NULL);
    default:
        if (isalpha(*lexer->peek) || *lexer->peek == '_') {
            return scan_id(lexer);
        } else if (isdigit(*lexer->peek)) {
            Token *tok = scan_numeric_literal(lexer, INTEGER_PART);
            if (tok != NULL) {
                return tok;
            }
        }

        error(lexer->meta, lexer->line, "illegal symbol %c\n", *lexer->peek);
        exit(EXIT_FAILURE);
    }
}

void start_buffering(Lexer *lexer) {
    lexer->buf = lexer->peek;
}

char *get_buffer(Lexer *lexer) {
    if (lexer->buf == NULL) {
        return NULL;
    }

    char *end = lexer->peek;

    size_t len = end - lexer->buf;
    char *lexeme = malloc(len + 1);
    memcpy(lexeme, lexer->buf, len + 1);
    *(lexeme + len) = '\0';

    lexer->buf = NULL;

    return lexeme;
}

void free_lexer(Lexer *lexer) {
    free(lexer);
}

static void single_line_comment(Lexer *lexer) {
    char c;
    do {
        c = next(lexer);
    } while (c != '\n' && c != EOF);
}

static void multi_line_comment(Lexer *lexer) {
    unsigned int line = lexer->line;
    int state = 0;

    char c;
    do {
        c = next(lexer);
        if (state == 0) {
            if (c == '*') {
                state = 1;
            }
        } else {
            if (c == '/') {
                state = 2;
            } else if (c != '*') {
                state = 0;
            }
        }
    } while (state != 2 && c != EOF);

    if (state == 2) {
        reset(lexer);
    } else {
        error(lexer->meta, line, "unclosed multi line comment starting at line %d\n", line);
        exit(EXIT_FAILURE);
    }
}

static Token *scan_id(Lexer *lexer) {
    start_buffering(lexer);

    char c = *lexer->peek;
    int underscore_only = c == '_';
    do {
        underscore_only &= c == '_';
        c = next(lexer);
    } while (isalnum(c) || c == '_');

    char *lexeme = get_buffer(lexer);

    if (underscore_only) {
        error(lexer->meta, lexer->line, "illegal symbol %s\n", lexeme);
        exit(EXIT_FAILURE);
    } else {
        Token *ret;
        if (strcmp(lexeme, "import") == 0) {
            ret = mktok(TOK_IMPORT);
        } else if (strcmp(lexeme, "fun") == 0) {
            ret = mktok(TOK_FUN);
        } else if (strcmp(lexeme, "private") == 0) {
            ret = mktok(TOK_PRIVATE);
        } else if (strcmp(lexeme, "native") == 0) {
            ret = mktok(TOK_NATIVE);
        } else if (strcmp(lexeme, "const") == 0) {
            ret = mktok(TOK_CONST);
        } else if (strcmp(lexeme, "var") == 0) {
            ret = mktok(TOK_VAR);
        } else if (strcmp(lexeme, "check") == 0) {
            ret = mktok(TOK_CHECK);
        } else {
            ret = mktokl(ID, lexeme);
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

Token *make_token(Lexer *lexer, int tag, char *lexeme, bool is_reset) {
    if (is_reset) {
        reset(lexer);
    }

    Token *tok = malloc(sizeof(Token));
    tok->line = lexer->line;
    tok->lexeme = lexeme;
    tok->tag = tag;

    return tok;
}

/*void lexer_err(Lexer *lexer, char *msg) {
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
}*/