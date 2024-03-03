#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "token.h"
#include "lexer.h"
#include "translator.h"
#include "lib/console.h"

static void move(translator *t) {
    memcpy(t->prev, t->look, sizeof(token));
    *t->look = scan(t->lexer);
    TOK_PRINT(t->look);
}

static void match(translator *t, int tag) {
    if (t->look->tag == tag) {
        if (t->look->tag != EOF) {
            move(t);
        }
    } else {
        print(E, "unexpected token, expected %d\n", tag);
        TOK_PRINT(t->look);
        print(E, "---");
    }
}

static void qidexprp(translator *t);

static char *qidp(translator *t, char *id) {
    switch (t->look->tag) {
    case DOT:
        match(t, DOT);
        strcat(id, ".");
        match(t, ID);
        id = realloc(id, strlen(id) + 1 /* for . */ + strlen(t->prev->lexeme) + 1 /* for \0 */);
        strcat(id, t->prev->lexeme);
        qidp(t, id);
        break;
    case PRIVATE:
    case ASSIGN:
    case IMPORT:
    case NATIVE:
    case COMMA:
    case CONST:
    case EOF:
    case FUN:
    case LPT:
    case RPT:
    case RPG:
    case ID:
        return id;
    default:
        print(E, "qidp");
        exit(EXIT_FAILURE);
    }
}

static char *qid(translator *t) {
    switch (t->look->tag) {
    case ID:
        match(t, ID);
        char *id = t->prev->lexeme;
        return qidp(t, id);
    default:
        print(E, "qid");
        exit(EXIT_FAILURE);
    }
}

static void expr(translator *t) {
    switch (t->look->tag) {
    case ID:
        qid(t);
        qidexprp(t);
        break;
    default:
        print(E, "expr");
        exit(EXIT_FAILURE);
    }
}

static void invokearglistp(translator *t) {
    switch (t->look->tag) {
    case COMMA:
        match(t, COMMA);
        expr(t);
        invokearglistp(t);
        break;
    case RPT:
        break;
    default:
        print(E, "invokearglistp");
        exit(EXIT_FAILURE);
    }
}

static void invokearglist(translator *t) {
    switch (t->look->tag) {
    case ID:
        expr(t);
        invokearglistp(t);
        break;
    case RPT:
        break;
    default:
        print(E, "invokearglist");
        exit(EXIT_FAILURE);
    }
}

static void invokeargs(translator *t) {
    switch (t->look->tag) {
    case LPT:
        match(t, LPT);
        invokearglist(t);
        match(t, RPT);
        break;
    default:
        print(E, "invokeargs");
        exit(EXIT_FAILURE);
    }
}

static void qidexprp(translator *t) {
    switch (t->look->tag) {
    case LPT:
        invokeargs(t);
        break;
    case PRIVATE:
    case NATIVE:
    case COMMA:
    case CONST:
    case EOF:
    case FUN:
    case RPG:
    case RPT:
    case ID:
        break;
    default:
        print(E, "qidexprp");
        exit(EXIT_FAILURE);
    }
}

static void qidstatp(translator *t) {
    switch (t->look->tag) {
    case ASSIGN:
        match(t, ASSIGN);
        expr(t);
        break;
    case LPT:
        invokeargs(t);
        break;
    default:
        print(E, "qidstatp");
        exit(EXIT_FAILURE);
    }
}

static void stat(translator *t) {
    switch (t->look->tag) {
    case CONST:
        match(t, CONST);
        match(t, ID);
        match(t, ASSIGN);
        expr(t);
        break;
    case ID:
        qid(t);
        qidstatp(t);
        break;
    default:
        print(E, "stat");
        exit(EXIT_FAILURE);
    }
}

static void statlistp(translator *t) {
    switch (t->look->tag) {
    case CONST:
    case ID:
        stat(t);
        statlistp(t);
        break;
    case RPG:
        break;
    default:
        print(E, "statlistp");
        exit(EXIT_FAILURE);
    }
}

static void statlist(translator *t) {
    switch (t->look->tag) {
    case LPG:
        match(t, LPG);
        statlistp(t);
        match(t, RPG);
        break;
    default:
        print(E, "statlist");
        exit(EXIT_FAILURE);
    }
}

static void funarg(translator *t) {
    switch (t->look->tag) {
    case CONST:
        match(t, CONST);
        match(t, ID);
        break;
    case ID:
        match(t, ID);
        break;
    default:
        print(E, "funarg");
        exit(EXIT_FAILURE);
    }
}

static void fundefarglistp(translator *t) {
    switch (t->look->tag) {
    case COMMA:
        match(t, COMMA);
        funarg(t);
        match(t, ASSIGN);
        expr(t);
        fundefarglistp(t);
        break;
    case RPT:
        break;
    default:
        print(E, "fundefarglistp");
        exit(EXIT_FAILURE);
    }
}

static void funarglistp(translator *t) {
    switch (t->look->tag) {
    case COMMA:
        match(t, COMMA);
        funarg(t);
        funarglistp(t);
        break;
    case ASSIGN:
        match(t, ASSIGN);
        expr(t);
        fundefarglistp(t);
        break;
    case RPT:
        break;
    default:
        print(E, "funarglistp");
        exit(EXIT_FAILURE);
    }
}

static void funarglist(translator *t) {
    switch (t->look->tag) {
    case CONST:
    case ID:
        funarg(t);
        funarglistp(t);
        break;
    case RPT:
        break;
    default:
        print(E, "funarglist");
        exit(EXIT_FAILURE);
    }
}

static void funsig(translator *t) {
    switch (t->look->tag) {
    case FUN:
        match(t, FUN);
        match(t, ID);
        match(t, LPT);
        funarglist(t);
        match(t, RPT);
        break;
    default:
        print(E, "funsig");
        exit(EXIT_FAILURE);
    }
}

static void fundecl(translator *t) {
    switch (t->look->tag) {
    case FUN:
        funsig(t);
        statlist(t);
        break;
    default:
        print(E, "fundecl");
        exit(EXIT_FAILURE);
    }
}

static void privatefilep(translator *t) {
    switch (t->look->tag) {
    case CONST:
        match(t, CONST);
        match(t, ID);
        match(t, ASSIGN);
        expr(t);
        break;
    case NATIVE:
        match(t, NATIVE);
        funsig(t);
        break;
    case FUN:
        fundecl(t);
        break;
    default:
        print(E, "privatefilep");
        exit(EXIT_FAILURE);
    }
}

static void filep(translator *t) {
    switch (t->look->tag) {
    case PRIVATE:
        match(t, PRIVATE);
        privatefilep(t);
        filep(t);
        break;
    case NATIVE:
        match(t, NATIVE);
        funsig(t);
        filep(t);
        break;
    case FUN:
        fundecl(t);
        filep(t);
        break;
    case CONST:
    case ID:
        stat(t);
        filep(t);
        break;
    case EOF:
        break;
    default:
        print(E, "filep");
        exit(EXIT_FAILURE);
    }
}

static void importlist(translator *t) {
    switch (t->look->tag) {
    case IMPORT: {
        match(t, IMPORT);
        char *id = qid(t);

        t->bin->import_count++;
        t->bin->imports = reallocarray(t->bin->imports, t->bin->import_count, sizeof(char *));
        t->bin->imports[t->bin->import_count - 1] = id;

        importlist(t);
        break;
    }
    case PRIVATE:
    case NATIVE:
    case CONST:
    case EOF:
    case FUN:
    case ID:
        break;
    default:
        print(E, "importlist");
        exit(EXIT_FAILURE);
    }
}

static void file(translator *t) {
    switch (t->look->tag) {
    case PRIVATE:
    case NATIVE:
    case IMPORT:
    case CONST:
    case EOF:
    case FUN:
    case ID:
        importlist(t);
        filep(t);
        match(t, EOF);
        break;
    default:
        print(E, "file");
        exit(EXIT_FAILURE);
    }
}

translator init_translator(lexer *lexer) {
    translator t;
    t.lexer = lexer;
    t.prev = malloc(sizeof(token));
    t.look = malloc(sizeof(token));
    t.bin = malloc(sizeof(bobo_bin));
    t.bin->imports = NULL;
    t.bin->import_count = 0;
    return t;
}

bobo_bin *translate(translator *translator) {
    move(translator);
    file(translator);
    return translator->bin;
}