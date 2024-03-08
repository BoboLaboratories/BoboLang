/* Modified sources from https://github.com/benhoyt/ht */

#ifndef BOBO_LANG_COMPILER_SYMBOL_TABLE_H
#define BOBO_LANG_COMPILER_SYMBOL_TABLE_H

#include <stdbool.h>
#include <sys/types.h>

/* Hash table structure: create with ht_create, free with ht_destroy. */
typedef struct st st;

/* Create hash table and return pointer to it, or NULL if out of memory. */
st *st_create(st *prev);

/* Free memory allocated for hash table, including allocated keys. */
void st_destroy(st *table);

/* Get item with given key (NUL-terminated) from hash table. Return */
/* value (which was set with ht_set), or NULL if key not found. */
void *st_get(st *table, const char *key);

/* Set item with given key (NUL-terminated) to value (which must not
   be NULL). If not already present in table, key is copied to newly
   allocated memory (keys are freed automatically when ht_destroy is
   called). Return address of copied key, or NULL if out of memory. */
const char *st_set(st *table, const char *key, void *value);

/* Return number of items in hash table. */
size_t st_length(st *table);

/* Hash table iterator: create with ht_iterator, iterate with ht_next. */
typedef struct {
    const char *key;  /* current key */
    void *value;      /* current value */

    /* Don't use these fields directly. */
    st *_table;       /* reference to hash table being iterated */
    size_t _index;    /* current index into ht._entries */
} sti;

/* Return new hash table iterator (for use with ht_next). */
sti st_iterator(st *table);

/* Move iterator to next item in hash table, update iterator's key */
/* and value to current item, and return true. If there are no more */
/* items, return false. Don't call ht_set during iteration. */
bool st_next(sti *it);

#endif