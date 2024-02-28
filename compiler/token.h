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
#define LPT     ((token) { '(', "(" })
#define RPT     ((token) { ')', ")" })
#define LPG     ((token) { '{', "{" })
#define RPG     ((token) { '}', "}" })
#define NOT     ((token) { '!', "!" })
#define LT      ((token) { '<', "<" })
#define GT      ((token) { '>', ">" })
#define DOT     ((token) { '.', "." })
#define COMMA   ((token) { ',', "," })
#define PLUS    ((token) { '+', "+" })
#define MINUS   ((token) { '-', "-" })
#define MUL     ((token) { '*', "*" })
#define DIV     ((token) { '/', "/" })
#define ASSIGN  ((token) { '=', "=" })

/* constant tokens non-mapped to extended ASCII */
#define ARROW   ((token) { 256, "->"       })
#define AND     ((token) { 257, "&&"       })
#define OR      ((token) { 258, "||"       })
#define LE      ((token) { 259, "<="       })
#define GE      ((token) { 260, ">="       })
#define EQ      ((token) { 261, "=="       })
#define NEQ     ((token) { 262, "!="       })
#define IMPORT  ((token) { 263, "import"   })
#define MODULE  ((token) { 264, "module"   })
#define FUN     ((token) { 265, "fun"      })
#define PRIVATE ((token) { 266, "private"  })
#define PUBLIC  ((token) { 267, "public"   })
#define CONST   ((token) { 268, "const"    })
#define VAR     ((token) { 269, "var"      })
#define CHECK   ((token) { 270, "check"    })

/* special tokens */
#define ERR     (-2)
#define TOK_EOF ((token) { EOF, "" })
#define TOK_ERR ((token) { ERR, "" })
#define ID      (271)

#endif