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

#include <string.h>
#include <malloc.h>

#include "meta.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "translator/translator.h"
#include "semantic/semantic_analyzer.h"

struct translator {
    Meta *meta;
    AST_Program *program;
};

Translator *init_translator(Meta *meta) {
    Translator *translator = malloc(sizeof(Translator));

    translator->meta = meta;
    translator->program = NULL;

    return translator;
}

static void disassemble(AST_Program *program);

BinaryModule *translate(Translator *translator) {
    Meta *meta = translator->meta;

    Lexer *lexer = init_lexer(meta);
    Parser *parser = init_parser(lexer);
    AST_Program *ast = parse(parser);

    SemanticAnalyzer *sem = init_semantic_analyzer(meta, ast);
    analyze(sem);

    disassemble(ast);
    return NULL;
}


static void print_expr(char *prefix, AST_Expr *node) {
    if (node != NULL) {
        switch (node->type) {
        case EXPR_QID: {
            printf("%s%s", prefix, (char *) node->value);
            break;
        }
        case EXPR_INVOKE: {
            int i;
            AST_ExprInvoke *invoke = node->value;
            printf("%s%s(", prefix, invoke->qid);
            if (invoke->args != NULL) {
                for (i = 0; i < al_size(invoke->args); i++) {
                    print_expr(i == 0 ? "" : ", ", al_get(invoke->args, i));
                }
            }
            printf(")");
            break;
        }
        case EXPR_NUMERIC_LITERAL: {
            printf("%s%g", prefix, *((double *) node->value));
        }
        default:
            break;
        }
    }
}

static void print_stat(char *prefix, AST_Stat *node) {
    if (node != NULL) {
        switch (node->type) {
        case STAT_INVOKE: {
            AST_StatInvoke *v = node->value;
            printf("%s%s(", prefix, v->qid);
            if (v->args != NULL) {
                int i;
                for (i = 0; i < al_size(v->args); i++) {
                    print_expr(i == 0 ? "" : ", ", al_get(v->args, i));
                }
            }
            printf(")");
            break;
        }
        case STAT_VAR_DECL: {
            AST_StatVarDecl *v = node->value;
            printf("%s", prefix);
            if (v->sig->is_private) printf("private ");
            printf(v->sig->is_const ? "const " : "var ");
            printf("%s", v->sig->name);
            print_expr(" = ", v->init);
            break;
        }
        case STAT_VAR_ASSIGN: {
            AST_StatVarAssign *v = node->value;
            printf("%s%s", prefix, v->name);
            print_expr(" = ", v->expr);
            break;
        }
        }
    }
    printf("\n");
}

static void disassemble(AST_Program *program) {
    printf("\n");

    int i, j;
    for (i = 0; i < al_size(program->imports); i++) {
        printf("import %s\n", (char *) al_get(program->imports, i));
    }
    printf("\n");

    for (i = 0; i < al_size(program->stats); i++) {
        AST_ProgramStat *ps = al_get(program->stats, i);
        switch (ps->type) {
        case PROGRAM_FUNDEF: {
            AST_FunDef *f = ps->value;
            if (f->sig->is_private) printf("private ");
            if (f->sig->is_native) printf("native ");
            printf("fun %s(", f->sig->name);
            if (f->sig->args != NULL) {
                for (j = 0; j < al_size(f->sig->args); j++) {
                    AST_FunArg *a = al_get(f->sig->args, j);
                    if (a->is_const) printf("const ");
                    printf("%s", a->name);
                    print_expr(" = ", a->expr);
                    if (j < al_size(f->sig->args) - 1) {
                        printf(", ");
                    }
                }
            }
            printf(")");
            if (!f->sig->is_native) printf(" {\n");
            if (f->stats != NULL) {
                for (j = 0; j < al_size(f->stats); j++) {
                    print_stat("\t", al_get(f->stats, j));
                }
            }
            if (!f->sig->is_native) printf("}");
            break;
        }
        case PROGRAM_STAT: {
            print_stat("", ps->value);
            break;
        }
        default:
            printf("FUCK U\n");
        }
        printf("\n");
    }
}
