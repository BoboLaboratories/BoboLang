#ifndef BOBO_LANG_COMPILER_PARSER_H
#define BOBO_LANG_COMPILER_PARSER_H

#include <stdio.h>

#include "ast.h"
#include "lexer/lexer.h"

typedef struct parser Parser;

Parser *init_parser(Lexer *lexer);
AST_Program *parse(Parser *parser);

#endif