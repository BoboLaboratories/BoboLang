#ifndef BOBO_LANG_COMPILER_PARSER_H
#define BOBO_LANG_COMPILER_PARSER_H

#include <stdio.h>

#include "ast.h"
#include "lexer/lexer.h"

typedef struct parser parser;

parser *init_parser(Lexer *lexer);
ast_program *parse(parser *parser);

#endif