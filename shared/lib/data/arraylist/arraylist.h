#ifndef BOBO_LANG_STRUCTURES_LIST_H
#define BOBO_LANG_STRUCTURES_LIST_H

#include <stdbool.h>

typedef struct array_list ArrayList;

typedef struct {
    ArrayList *list;
    unsigned long index;
} ArrayListIterator;

ArrayList *al_create(unsigned long max_size);

bool al_add(ArrayList *list, void *elem);

void *al_get(ArrayList *list, unsigned long index);

bool al_is_full(ArrayList *list);

unsigned long al_size(ArrayList *list);

unsigned long al_max_size(ArrayList *list);

ArrayListIterator al_iterator(ArrayList *list);

void *al_iterator_next(ArrayListIterator *iterator);

#endif