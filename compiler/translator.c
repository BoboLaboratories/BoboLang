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

static void expr() {
    switch (look.tag) {
    case ID:
        match(ID);
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
    case VAR:
        match(VAR);
        break;
    case ID:
        break;
    default:
        print(E, "varmod");
        exit(EXIT_FAILURE);
    }
}

static void var() {
    switch (look.tag) {
    case CONST:
    case VAR:
    case ID:
        varmod();
        match(ID);
        break;
    default:
        print(E, "var");
        exit(EXIT_FAILURE);
    }
}

static void assignmentp() {
    switch (look.tag) {
    case COMMA:
        match(COMMA);
        match(ID);
        assignmentp();
        break;
    case ASSIGN:
        match(ASSIGN);
        expr();
        break;
    default:
        print(E, "assignmentp");
        exit(EXIT_FAILURE);
    }
}

static void assignment() {
    switch (look.tag) {
    case CONST:
    case VAR:
    case ID:
        varmod();
        match(ID);
        assignmentp();
        break;
    default:
        print(E, "assignment");
        exit(EXIT_FAILURE);
    }
}

static void funmod() {
    switch (look.tag) {
    case PRIVATE:
        match(PRIVATE);
        break;
    case PUBLIC:
        match(PUBLIC);
        break;
    case FUN:
        break;
    default:
        print(E, "funmod");
        exit(EXIT_FAILURE);
    }
}

static void stat() {

    switch (look.tag) {
    case CONST:
    case VAR:
    case ID:
        assignment();
        break;
    default:
        print(E, "stat");
        exit(EXIT_FAILURE);
    }
}

static void statlist() {
    switch (look.tag) {
    case CONST:
    case VAR:
    case ID:
        stat();
        statlist();
        break;
    case RPG:
        break;
    default:
        print(E, "statlist");
        exit(EXIT_FAILURE);
    }
}

static void defarglistp() {
    switch (look.tag) {
    case COMMA:
        match(COMMA);
        var();
        match(ASSIGN);
        expr();
        defarglistp();
        break;
    case RPT:
        break;
    default:
        print(E, "defarglistp");
        exit(EXIT_FAILURE);
    }
}

static void arglistp() {
    switch (look.tag) {
    case COMMA:
        match(COMMA);
        var();
        arglistp();
        break;
    case ASSIGN:
        match(ASSIGN);
        expr();
        defarglistp();
        break;
    case RPT:
        break;
    default:
        print(E, "arglistp");
        exit(EXIT_FAILURE);
    }
}

static void arglist() {
    switch (look.tag) {
    case CONST:
    case VAR:
    case ID:
        var();
        arglistp();
        break;
    case RPT:
        break;
    default:
        print(E, "arglist");
        exit(EXIT_FAILURE);
    }
}

static void fun() {
    switch (look.tag) {
    case PRIVATE:
    case PUBLIC:
    case FUN:
        funmod();
        match(FUN);
        match(ID);
        match(LPT);
        arglist();
        match(RPT);
        match(LPG);
        statlist();
        match(RPG);
        break;
    default:
        print(E, "fun");
        exit(EXIT_FAILURE);
    }
}

static void modulep() {
    switch (look.tag) {
    case CONST:
    case VAR:
    case ID:
        assignment();
        modulep();
        break;
    case PRIVATE:
    case PUBLIC:
    case FUN:
        fun();
        break;
    case RPG:
        break;
    default:
        print(E, "modulep");
        exit(EXIT_FAILURE);
    }
}

static void scriptp() {

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
    case PUBLIC:
    case CONST:
    case VAR:
    case FUN:
    case ID:
        scriptp();
        break;
    default:
        print(E, "filep");
        exit(EXIT_FAILURE);
    }
}

static void importidp() {
    switch (look.tag) {
    case DOT:
        match(DOT);
        match(ID);
        importidp();
        break;
    case IMPORT:
    case PRIVATE:
    case PUBLIC:
    case CONST:
    case VAR:
    case MODULE:
    case FUN:
    case ID:
        break;
    default:
        print(E, "importidp");
        exit(EXIT_FAILURE);
    }
}

static void importlist() {
    switch (look.tag) {
    case IMPORT:
        match(IMPORT);
        match(ID);
        importidp();
        importlist();
        break;
    case PRIVATE:
    case PUBLIC:
    case CONST:
    case VAR:
    case MODULE:
    case FUN:
    case ID:
        break;
    default:
        print(E, "importlist");
        exit(EXIT_FAILURE);
    }
}

static void file() {
    importlist();
    filep();
}

void translate(FILE *f) {
    fptr = f;
    move();

    file();
}