#include <stdlib.h>

#include "lexer.h"
#include "translator.h"
#include "lib/console.h"

static token look;
static FILE *fptr;

static void move() {
    look = scan(fptr);
}

static void match(int tag) {
    if (look.tag == tag) {
        TOK_PRINT(look);
        if (look.tag != EOF) {
            move();
        }
    } else {
        print(E, "unexpected token, expected %d\n", tag);
        TOK_PRINT(look);
        print(E, "---");
    }
}

static void qidp() {
    switch (look.tag) {
    case DOT:
        match(DOT);
        match(ID);
        qidp();
        break;
    case PRIVATE:
    case ASSIGN:
    case IMPORT:
    case MODULE:
    case NATIVE:
    case COMMA:
    case CONST:
    case EOF:
    case FUN:
    case LPT:
    case RPT:
    case RPG:
    case ID:
        break;
    default:
        print(E, "qidp");
        exit(EXIT_FAILURE);
    }
}

static void qid() {
    switch (look.tag) {
    case ID:
        match(ID);
        qidp();
        break;
    default:
        print(E, "qid");
        exit(EXIT_FAILURE);
    }
}

static void invokearglistp() {
    switch (look.tag) {
    case COMMA:
        match(COMMA);
        qid();
        invokearglistp();
        break;
    case RPT:
        break;
    default:
        print(E, "invokearglistp");
        exit(EXIT_FAILURE);
    }
}

static void invokearglist() {
    switch (look.tag) {
    case ID:
        qid();
        invokearglistp();
        break;
    case RPT:
        break;
    default:
        print(E, "invokearglist");
        exit(EXIT_FAILURE);
    }
}

static void invokeargs() {
    switch (look.tag) {
    case LPT:
        match(LPT);
        invokearglist();
        match(RPT);
        break;
    default:
        print(E, "invokeargs");
        exit(EXIT_FAILURE);
    }
}

static void qidexprp() {
    switch (look.tag) {
    case LPT:
        invokeargs();
        break;
    case PRIVATE:
    case NATIVE:
    case COMMA:
    case CONST:
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

static void expr() {
    switch (look.tag) {
    case ID:
        qid();
        qidexprp();
        break;
    default:
        print(E, "expr");
        exit(EXIT_FAILURE);
    }
}

static void varmod() {
    switch (look.tag) {
    case CONST:
        match(CONST);
        break;
    case ID:
        break;
    default:
        print(E, "varmod");
        exit(EXIT_FAILURE);
    }
}

static void statassignp() {
    switch (look.tag) {
    case ASSIGN:
        match(ASSIGN);
        expr();
        break;
    default:
        print(E, "statassignp");
        exit(EXIT_FAILURE);
    }
}

static void qidinnerstatp() {
    switch (look.tag) {
    case ASSIGN:
        statassignp();
        break;
    case LPT:
        invokeargs();
        break;
    default:
        print(E, "qidinnerstatp");
        exit(EXIT_FAILURE);
    }
}

static void innerstat() {
    switch (look.tag) {
    case CONST:
        match(CONST);
        qid();
        statassignp();
        break;
    case ID:
        qid();
        qidinnerstatp();
        break;
    default:
        print(E, "innerstat");
        exit(EXIT_FAILURE);
    }
}

static void outerstat() {
    switch (look.tag) {
    case CONST:
    case ID:
        varmod();
        qid();
        statassignp();
        break;
    default:
        print(E, "outerstat");
        exit(EXIT_FAILURE);
    }
}

static void statlistp() {
    switch (look.tag) {
    case CONST:
    case ID:
        innerstat();
        statlistp();
        break;
    case RPG:
        break;
    default:
        print(E, "statlistp");
        exit(EXIT_FAILURE);
    }
}

static void statlist() {
    switch (look.tag) {
    case LPG:
        match(LPG);
        statlistp();
        match(RPG);
        break;
    default:
        print(E, "statlist");
        exit(EXIT_FAILURE);
    }
}

static void funarg() {
    switch (look.tag) {
    case CONST:
    case ID:
        varmod();
        match(ID);
        break;
    default:
        print(E, "funarg");
        exit(EXIT_FAILURE);
    }
}

static void fundefarglistp() {
    switch (look.tag) {
    case COMMA:
        match(COMMA);
        funarg();
        match(ASSIGN);
        expr();
        fundefarglistp();
        break;
    case RPT:
        break;
    default:
        print(E, "fundefarglistp");
        exit(EXIT_FAILURE);
    }
}

static void funarglistp() {
    switch (look.tag) {
    case COMMA:
        match(COMMA);
        funarg();
        funarglistp();
        break;
    case ASSIGN:
        match(ASSIGN);
        expr();
        fundefarglistp();
        break;
    case RPT:
        break;
    default:
        print(E, "funarglistp");
        exit(EXIT_FAILURE);
    }
}

static void funarglist() {
    switch (look.tag) {
    case CONST:
    case ID:
        funarg();
        funarglistp();
        break;
    case RPT:
        break;
    default:
        print(E, "funarglist");
        exit(EXIT_FAILURE);
    }
}

static void funsig() {
    switch (look.tag) {
    case FUN:
        match(FUN);
        match(ID);
        match(LPT);
        funarglist();
        match(RPT);
        break;
    default:
        print(E, "funsig");
        exit(EXIT_FAILURE);
    }
}

static void modulefunp() {
    switch (look.tag) {
    case NATIVE:
        match(NATIVE);
        funsig();
        break;
    case FUN:
        funsig();
        statlist();
        break;
    default:
        print(E, "modulefunp");
        exit(EXIT_FAILURE);
    }
}

static void funmod() {
    switch (look.tag) {
    case PRIVATE:
        match(PRIVATE);
        break;
    case NATIVE:
    case FUN:
        break;
    default:
        print(E, "funmod");
        exit(EXIT_FAILURE);
    }
}

static void modulefun() {
    switch (look.tag) {
    case PRIVATE:
    case NATIVE:
    case FUN:
        funmod();
        modulefunp();
        break;
    default:
        print(E, "modulefun");
        exit(EXIT_FAILURE);
    }
}

static void modulep() {
    switch (look.tag) {
    case CONST:
    case ID:
        outerstat();
        modulep();
        break;
    case PRIVATE:
    case NATIVE:
    case FUN:
        modulefun();
        modulep();
        break;
    case RPG:
        break;
    default:
        print(E, "modulep");
        exit(EXIT_FAILURE);
    }
}

static void scriptp() {
    /* TODO */
}

static void filep() {
    switch (look.tag) {
    case MODULE:
        match(MODULE);
        match(ID);
        match(LPG);
        modulep();
        match(RPG);
        break;
    case PRIVATE:
    case CONST:
    case EOF:
    case FUN:
    case ID:
        scriptp();
        break;
    default:
        print(E, "filep");
        exit(EXIT_FAILURE);
    }
}

static void importlist() {
    switch (look.tag) {
    case IMPORT:
        match(IMPORT);
        qid();
        importlist();
        break;
    case PRIVATE:
    case MODULE:
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

static void file() {
    switch (look.tag) {
    case PRIVATE:
    case IMPORT:
    case MODULE:
    case CONST:
    case EOF:
    case FUN:
    case ID:
        importlist();
        filep();
        match(EOF);
        break;
    default:
        print(E, "file");
        exit(EXIT_FAILURE);
    }
}

void translate(FILE *f) {
    fptr = f;
    move();

    file();
}