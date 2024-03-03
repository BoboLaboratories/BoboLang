#ifndef BOBO_LANG_COMPILER_TOKEN_H
#define BOBO_LANG_COMPILER_TOKEN_H

typedef struct {
    int tag;
    int line;
    char *lexeme;
} token;

#define TOK_PRINT(tok)  {                                                       \
                            if (tok->tag == EOF) {                             \
                                print(N, "<EOF>\n");                            \
                            } else if (tok->tag != ERR) {                      \
                                print(N, "<%d, %s>\n", tok->tag, tok->lexeme);  \
                            }                                                   \
                        }


/* constant tokens mapped to extended ASCII */
#define LPT    '('
#define RPT    ')'
#define LPG    '{'
#define RPG    '}'
#define NOT    '!'
#define LT     '<'
#define GT     '>'
#define DOT    '.'
#define COMMA  ','
#define PLUS   '+'
#define MINUS  '-'
#define MUL    '*'
#define DIV    '/'
#define ASSIGN '='

#define TOK_LPT     LPT,    "("
#define TOK_RPT     RPT,    ")"
#define TOK_LPG     LPG,    "{"
#define TOK_RPG     RPG,    "}"
#define TOK_NOT     NOT,    "!"
#define TOK_LT      LT,     "<"
#define TOK_GT      GT,     ">"
#define TOK_DOT     DOT,    "."
#define TOK_COMMA   COMMA,  ","
#define TOK_PLUS    PLUS,   "+"
#define TOK_MINUS   MINUS,  "-"
#define TOK_MUL     MUL,    "*"
#define TOK_DIV     DIV,    "/"
#define TOK_ASSIGN  ASSIGN, "="

/* constant tokens non-mapped to extended ASCII */
#define ARROW   256
#define AND     257
#define OR      258
#define LE      259
#define GE      260
#define EQ      261
#define NEQ     262
#define IMPORT  263
#define FUN     264
#define PRIVATE 265
#define NATIVE  266
#define CONST   267
#define CHECK   268

#define TOK_ARROW   ARROW,   "->"
#define TOK_AND     AND,     "&&"
#define TOK_OR      OR,      "||"
#define TOK_LE      LE,      "<="
#define TOK_GE      GE,      ">="
#define TOK_EQ      EQ,      "=="
#define TOK_NEQ     NEQ,     "!="
#define TOK_IMPORT  IMPORT,  "import"
#define TOK_FUN     FUN,     "fun"
#define TOK_PRIVATE PRIVATE, "private"
#define TOK_NATIVE  NATIVE,  "native"
#define TOK_CONST   CONST,   "const"
#define TOK_CHECK   CHECK,   "check"

/* special tokens */
#define ID      269
#define TOK_ID  ID, lexeme

#define ERR     (-2)
#define TOK_ERR ERR, ""

#define TOK_EOF EOF, ""



#endif