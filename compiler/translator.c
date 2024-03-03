#include <stdlib.h>
#include <string.h>
#include <malloc.h>

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

static void qidexprp();

static char *qidp(char *id) {
    switch (look.tag) {
    case DOT:
        match(DOT);
        strcat(id, ".");
        strcat(id, look.lexeme);
        match(ID);
        qidp(id);
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

static char *qid() {
    switch (look.tag) {
    case ID: {
        char *id = look.lexeme;
        match(ID);
        qidp(id);
        return id;
    }
    default:
        print(E, "qid");
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

static void invokearglistp() {
    switch (look.tag) {
    case COMMA:
        match(COMMA);
        expr();
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
        expr();
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

static void qidstatp() {
    switch (look.tag) {
    case ASSIGN:
        match(ASSIGN);
        expr();
        break;
    case LPT:
        invokeargs();
        break;
    default:
        print(E, "qidstatp");
        exit(EXIT_FAILURE);
    }
}

static void stat() {
    switch (look.tag) {
    case CONST:
        match(CONST);
        match(ID);
        match(ASSIGN);
        expr();
        break;
    case ID:
        qid();
        qidstatp();
        break;
    default:
        print(E, "stat");
        exit(EXIT_FAILURE);
    }
}

static void statlistp() {
    switch (look.tag) {
    case CONST:
    case ID:
        stat();
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
        match(CONST);
        match(ID);
        break;
    case ID:
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

static void fundecl() {
    TOK_PRINT(look);
    switch (look.tag) {
    case FUN:
        funsig();
        statlist();
        break;
    default:
        print(E, "fundecl");
        exit(EXIT_FAILURE);
    }
}

static void privatefilep() {
    switch (look.tag) {
    case CONST:
        match(CONST);
        match(ID);
        match(ASSIGN);
        expr();
        break;
    case NATIVE:
        match(NATIVE);
        funsig();
        break;
    case FUN:
        fundecl();
        break;
    default:
        print(E, "privatefilep");
        exit(EXIT_FAILURE);
    }
}

static void filep() {
    switch (look.tag) {
    case PRIVATE:
        match(PRIVATE);
        privatefilep();
        filep();
        break;
    case NATIVE:
        match(NATIVE);
        funsig();
        filep();
        break;
    case FUN:
        fundecl();
        filep();
        break;
    case CONST:
    case ID:
        stat();
        filep();
        break;
    case EOF:
        break;
    default:
        print(E, "filep");
        exit(EXIT_FAILURE);
    }
}

static AST_importlist *importlist(AST_importlist *list) {
    if (list == NULL) {
        list = malloc(sizeof(AST_importlist));
        list->import_count = 0;
        list->imports = NULL;
    }

    switch (look.tag) {
    case IMPORT:
        match(IMPORT);
        char *import = qid();
        list->import_count += 1;
        list->imports = reallocarray(list->imports, list->import_count, sizeof(char *));
        list->imports[list->import_count - 1] = import;
        importlist(list);
        break;
    case PRIVATE:
    case NATIVE:
    case CONST:
    case EOF:
    case FUN:
    case ID:
        return list;
    default:
        print(E, "importlist");
        exit(EXIT_FAILURE);
    }
}

static void file() {
    AST_file file;
    switch (look.tag) {
    case PRIVATE:
    case NATIVE:
    case IMPORT:
    case CONST:
    case EOF:
    case FUN:
    case ID:
        file.importlist = importlist(NULL);
        filep();
        match(EOF);
        {
            print(D, "Files to be imported:\n");
            int i = 0;
            while (i < file.importlist->import_count) {
                print(D, "\t%s\n", file.importlist->imports[i]);
                i++;
            }
        }
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