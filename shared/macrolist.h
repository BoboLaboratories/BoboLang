#ifndef BOBO_LANG_MACROLIST_H
#define BOBO_LANG_MACROLIST_H

/*
#define LIST_DEF(type, count_type, name)                                    \
    struct _##name {                                                        \
        count_type count;                                                   \
        type *values;                                                       \
    } *name

#define LIST_INIT(holder, name)                                             \
    holder->name = malloc(sizeof(struct _##name));                          \
    holder->name->count = 0;                                                \
    holder->name->values = NULL

#define LIST_ADD(dest, val)                                                 \
    dest->count++;                                                          \
    dest->values = reallocarray(dest->values, dest->count, sizeof(void *)); \
    dest->values[dest->count - 1] = val

*/

#endif