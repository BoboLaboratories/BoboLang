/*
 * MIT License
 *
 * Copyright (c) 2021 Ben Hoyt
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

/*
 * Modified sources from https://github.com/benhoyt/ht
 */

#ifndef BOBO_LANG_COMPILER_SYMBOL_TABLE_H
#define BOBO_LANG_COMPILER_SYMBOL_TABLE_H

#include <stdbool.h>
#include <sys/types.h>

/*
 * Hash table structure: create with ht_create, free with ht_destroy.
 */
typedef struct hash_table HashTable;

/*
 * Create hash table and return pointer to it, or NULL if out of memory.
 */
HashTable *ht_create(void);

/*
 * Free memory allocated for hash table, including allocated keys.
 */
void ht_destroy(HashTable *table);

/*
 * Get item with given key (NUL-terminated) from hash table.
 * Return expr (which was set with ht_set), or NULL if key not found.
 */
void *ht_get(HashTable *table, const char *key);

/*
 * Set item with given key (NUL-terminated) to expr (which must not be NULL).
 * If not already present in table, key is copied to newly allocated memory
 * (keys are freed automatically when ht_destroy is called).
 * Return address of copied key, or NULL if out of memory.
 */
const char *ht_set(HashTable *table, const char *key, void *value);

/*
 * Return number of items in hash table.
 */
size_t ht_length(HashTable *table);

/*
 * Hash table iterator: create with ht_iterator, iterate with ht_next.
 */
typedef struct {
    const char *key;  /* current key */
    void *value;      /* current expr */

    /* Don't use these fields directly. */
    HashTable *_table;       /* reference to hash table being iterated */
    size_t _index;    /* current index into ht._entries */
} HashTableIterator;

/*
 * Return new hash table iterator (for use with ht_next).
 */
HashTableIterator ht_iterator(HashTable *table);

/*
 * Move iterator to next item in hash table, update iterator's key
 * and expr to current item, and return true. If there are no more
 * items, return false. Don't call ht_set during iteration.
 */
bool ht_next(HashTableIterator *it);

#endif