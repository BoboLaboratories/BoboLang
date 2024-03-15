#ifndef BOBO_LANG_COMPILER_SEMANTIC_ANALYZER_H
#define BOBO_LANG_COMPILER_SEMANTIC_ANALYZER_H

#include "meta.h"
#include "parser/ast.h"
#include "lang/binary/module.h"

typedef struct semantic_analyzer SemanticAnalyzer;

SemanticAnalyzer *init_semantic_analyzer(Meta *meta, AST_Program *ast);

void analyze(SemanticAnalyzer *analyzer);

#endif