#ifndef BOBO_LANG_COMPILER_TOKEN_H
#define BOBO_LANG_COMPILER_TOKEN_H

typedef struct {
    int tag;
    char *lexeme;
} token;

#define TOK_PRINT(tok)  {                                                       \
                            if (tok.tag == EOF) {                               \
                                print(N, "<EOF>\n");                            \
                            } else if (tok.tag != ERR) {                        \
                                print(N, "<%d, %s>\n", tok.tag, tok.lexeme);    \
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

#define TOK_LPT     ((token) { LPT,    "(" })
#define TOK_RPT     ((token) { RPT,    ")" })
#define TOK_LPG     ((token) { LPG,    "{" })
#define TOK_RPG     ((token) { RPG,    "}" })
#define TOK_NOT     ((token) { NOT,    "!" })
#define TOK_LT      ((token) { LT,     "<" })
#define TOK_GT      ((token) { GT,     ">" })
#define TOK_DOT     ((token) { DOT,    "." })
#define TOK_COMMA   ((token) { COMMA,  "," })
#define TOK_PLUS    ((token) { PLUS,   "+" })
#define TOK_MINUS   ((token) { MINUS,  "-" })
#define TOK_MUL     ((token) { MUL,    "*" })
#define TOK_DIV     ((token) { DIV,    "/" })
#define TOK_ASSIGN  ((token) { ASSIGN, "=" })

/* constant tokens non-mapped to extended ASCII */
#define ARROW   256
#define AND     257
#define OR      258
#define LE      259
#define GE      260
#define EQ      261
#define NEQ     262
#define IMPORT  263
#define MODULE  264
#define FUN     265
#define PRIVATE 266
#define PUBLIC  267
#define CONST   268
#define VAR     269
#define CHECK   270

#define TOK_ARROW   ((token) { ARROW,   "->"       })
#define TOK_AND     ((token) { AND,     "&&"       })
#define TOK_OR      ((token) { OR,      "||"       })
#define TOK_LE      ((token) { LE,      "<="       })
#define TOK_GE      ((token) { GE,      ">="       })
#define TOK_EQ      ((token) { EQ,      "=="       })
#define TOK_NEQ     ((token) { NEQ,     "!="       })
#define TOK_IMPORT  ((token) { IMPORT,  "import"   })
#define TOK_MODULE  ((token) { MODULE,  "module"   })
#define TOK_FUN     ((token) { FUN,     "fun"      })
#define TOK_PRIVATE ((token) { PRIVATE, "private"  })
#define TOK_PUBLIC  ((token) { PUBLIC,  "public"   })
#define TOK_CONST   ((token) { CONST,   "const"    })
#define TOK_VAR     ((token) { VAR,     "var"      })
#define TOK_CHECK   ((token) { CHECK,   "check"    })

/* special tokens */
#define ID      271
#define ERR     (-2)
#define TOK_ERR ((token) { ERR, "" })
#define TOK_EOF ((token) { EOF, "" })


#endif