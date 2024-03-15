#include <malloc.h>

#include "lib/symboltable/symboltable.h"
#include "lib/data/hashtable/hashtable.h"

struct symbol_table {
    HashTable *ht;
    SymbolTable *prev;
};

SymbolTable *st_push(SymbolTable *prev) {
    SymbolTable *st = malloc(sizeof(SymbolTable));

    st->ht = ht_create();
    st->prev = prev;

    return st;
}

SymbolTable *st_pop(SymbolTable *curr) {
    return curr->prev;
}

void *st_get(SymbolTable *table, const char *key) {
    void *value = ht_get(table->ht, key);

    if (value == NULL && table->prev != NULL) {
        value = st_get(table->prev, key);
    }

    return value;
}

const char *st_set(SymbolTable *table, const char *key, void *value) {
    return ht_set(table->ht, key, value);
}