#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

#include "lexer/token.h"
#include "lexer/lexer.h"
#include "lib/console.h"

struct lexer {
    FILE *fptr;
    long buf_start;
    char peek;
    int line;
};


#define DFA_NUM_Q3 3


/* reads the next char from file */
#define next()                  (lexer->peek = fgetc(lexer->fptr))

/* make token with well known lexeme */
#define mktok(tok)              make_token(lexer, tok, false)

/* make token with well known lexeme and reset peek */
#define mktokr(tok)             make_token(lexer, tok, true)

/* make token with the given tag and lexeme */
#define mktokl(tag, lexeme)     make_token(lexer, tag, lexeme, false)

/* make token with the given tag and lexeme and reset peek */
#define mktoklr(tag, lexeme)    make_token(lexer, tag, lexeme, true)


static token *make_token(Lexer *lexer, int tag, char *lexeme, bool reset);

static void start_buffering(Lexer *lexer);

static char *get_buffer(Lexer *lexer);

static void single_line_comment(Lexer *lexer);

static void multi_line_comment(Lexer *lexer);

static token *scan_id(Lexer *lexer);

static token *scan_numeric_literal(Lexer *lexer, int state);

static int is_whitespace(Lexer *lexer);


Lexer *init_lexer(FILE *fptr) {
    Lexer *lexer = malloc(sizeof(Lexer));

    lexer->buf_start = -1;
    lexer->fptr = fptr;
    lexer->peek = ' ';
    lexer->line = 1;

    return lexer;
}

token *scan(Lexer *lexer) {
    while (is_whitespace(lexer)) {
        if (lexer->peek == '\n') {
            lexer->line++;
        }
        next();
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
        token *numeric_literal = scan_numeric_literal(lexer, 3);
        return numeric_literal != NULL ? numeric_literal : mktok(TOK_DOT);
    }
    case '*':
        return mktokr(TOK_MUL);
    case '+': {
        token *numeric_literal = scan_numeric_literal(lexer, 1);
        return numeric_literal != NULL ? numeric_literal : mktok(TOK_PLUS);
    }
    case '/':
        switch (next()) {
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
        return (next() == '=') ? mktokr(TOK_LE) : mktok(TOK_LT);
    case '>':
        return (next() == '=') ? mktokr(TOK_GE) : mktok(TOK_GT);
    case '!':
        return (next() == '=') ? mktokr(TOK_NEQ) : mktok(TOK_NOT);
    case '=':
        return (next() == '=') ? mktokr(TOK_EQ) : mktok(TOK_ASSIGN);
    case '-':
        return (next() == '>') ? mktokr(TOK_ARROW) : mktok(TOK_MINUS);
    case '&':
        if (next() != '&') {
            print(E, "illegal symbol &%c at line %d\n", lexer->peek, lexer->line);
            exit(EXIT_FAILURE);
        }
        return mktokr(TOK_AND);
    case '|':
        if (next() != '|') {
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
            token *tok = scan_numeric_literal(lexer, 2);
            if (tok != NULL) {
                return tok;
            }
        }

        DEBUG_BREAKPOINT;

        print(E, "erroneous symbol %c at line %d\n", lexer->peek, lexer->line);
        exit(EXIT_FAILURE);
    }
}

static void start_buffering(Lexer *lexer) {
    lexer->buf_start = ftell(lexer->fptr) - 1;
}

static char *get_buffer(Lexer *lexer) {
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
        next();
    } while (lexer->peek != '\n' && lexer->peek != EOF);
}

static void multi_line_comment(Lexer *lexer) {
    int line = lexer->line;
    int state = 0;

    do {
        next();
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
        next();
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

static token *scan_numeric_literal(Lexer *lexer, int state) {
    start_buffering(lexer);

    bool stop = false;
    while (!stop && state >= 0) {
        const char c = next();

        switch (state) {
        case 1:
            if (c == '.')
                state = 3;
            else if (isdigit(c))
                state = 2;
            else if (is_whitespace(lexer))
                stop = true;
            else
                state = -1;
            break;
        case 2:
            if (c == 'e')
                state = 5;
            else if (c == '.')
                state = 3;
            else if (isdigit(c))
                state = 2;
            else if (is_whitespace(lexer))
                stop = true;
            else
                state = -1;
            break;
        case 3:
            if (isdigit(c))
                state = 4;
            else if (c == '.') {
                state = 4;
                stop = true;
            } else if (is_whitespace(lexer))
                stop = true;
            else
                state = -1;
            break;
        case 4:
            if (c == 'e')
                state = 5;
            else if (isdigit(c))
                state = 4;
            else if (is_whitespace(lexer))
                stop = true;
            else
                state = -1;
            break;
        case 5:
            if (c == '.')
                state = 8;
            if (c == '+' || c == '-')
                state = 6;
            else if (isdigit(c))
                state = 7;
            else if (is_whitespace(lexer))
                stop = true;
            else
                state = -1;
            break;
        case 6:
            if (c == '.')
                state = 8;
            else if (isdigit(c))
                state = 7;
            else if (is_whitespace(lexer))
                stop = true;
            else
                state = -1;
            break;
        case 7:
            if (c == '.')
                state = 8;
            else if (isdigit(c))
                state = 7;
            else if (is_whitespace(lexer))
                stop = true;
            else
                state = -1;
            break;
        case 8:
            if (isdigit(c))
                state = 9;
            else if (is_whitespace(lexer))
                stop = true;
            else
                state = -1;
            break;
        case 9:
            if (isdigit(c))
                state = 9;
            else if (is_whitespace(lexer))
                stop = true;
            else
                state = -1;
            break;
        }

        printf("(char: %d, state: %d)\n", (int) lexer->peek, state);
    }

    if (state == 2 || state == 4 || state == 7 || state == 9) {
        char *lexeme = get_buffer(lexer);
        return mktokl(NUM, lexeme);
    } else {
        return NULL;
    }
}

static int is_whitespace(Lexer *lexer) {
    return lexer->peek == ' '
           || lexer->peek == '\t'
           || lexer->peek == '\n'
           || lexer->peek == '\r';
}

static token *make_token(Lexer *lexer, int tag, char *lexeme, bool reset) {
    if (reset) {
        lexer->peek = ' ';
    }

    token *tok = malloc(sizeof(token));
    tok->line = lexer->line;
    tok->lexeme = lexeme;
    tok->tag = tag;

    return tok;
}