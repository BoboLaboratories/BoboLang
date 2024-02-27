#ifndef BOBO_COMPILER_TAG_H
#define BOBO_COMPILER_TAG_H

typedef struct _token {
    const int tag;
    const char *lexeme;
} token;

// constant tokens mapped to extended ASCII
const token LPT     = { '(', "(" };
const token RPT     = { ')', ")" };
const token LPG     = { '{', "{" };
const token RPG     = { '}', "}" };
const token DOT     = { '.', "." };
const token COMMA   = { ',', "," };
const token PLUS    = { '+', "+" };
const token MINUS   = { '-', "-" };
const token MUL     = { '*', "*" };
const token DIV     = { '/', "/" };
const token ASSIGN  = { '=', "=" };

// constant tokens non-mapped to extended ASCII
const token ARROW   = { 256, "->"       };
const token NOT     = { 257, "!"        };
const token AND     = { 258, "&&"       };
const token OR      = { 259, "||"       };
const token LT      = { 260, "<"        };
const token GT      = { 261, ">"        };
const token LE      = { 262, "<="       };
const token GE      = { 263, ">="       };
const token EQ      = { 264, "=="       };
const token NEQ     = { 265, "!="       };
const token IMPORT  = { 266, "import"   };
const token MODULE  = { 267, "module"   };
const token FUN     = { 268, "fun"      };
const token PRIVATE = { 269, "private"  };
const token PUBLIC  = { 270, "public"   };
const token CONST   = { 271, "const"    };
const token VAR     = { 272, "var"      };
const token CHECK   = { 273, "check"    };

// special cases
const token EOF     = { -1, "" };
#define ID  274

#endif