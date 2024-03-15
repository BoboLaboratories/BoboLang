#include <stdbool.h>
#include <malloc.h>
#include <errno.h>

#include "arraylist.h"

struct array_list {
    unsigned long max_size;
    unsigned long size;
    void **values;
    unsigned long capacity;
};

ArrayList *al_create(unsigned long max_size) {
    ArrayList *list = malloc(sizeof(ArrayList));

    list->max_size = max_size;
    list->values = NULL;
    list->capacity = 0;
    list->size = 0;

    return list;
}

bool al_add(ArrayList *list, void *elem) {
    if (al_is_full(list)) {
        return false;
    }

    if (list->size >= list->capacity) {
        list->capacity = (list->capacity != 0) ? (2 * list->capacity) : 1;
        list->values = reallocarray(list->values, list->capacity, sizeof(void *));
        if (errno == ENOMEM) {
            return false;
        }
    }

    list->values[list->size] = elem;
    list->size++;
    return true;
}

void *al_get(ArrayList *list, unsigned long index) {
    void *elem = NULL;
    if (index <= al_size(list)) {
        elem = list->values[index];
    }
    return elem;
}

bool al_is_full(ArrayList *list) {
    return al_size(list) >= al_max_size(list);
}

unsigned long al_size(ArrayList *list) {
    return list->size;
}

unsigned long al_max_size(ArrayList *list) {
    return list->max_size;
}

void list_free(ArrayList *list) {
    int i;
    for (i = 0; i < al_size(list); i++) {
        free(list->values[i]);
    }
    free(list);
}

ArrayListIterator al_iterator(ArrayList *list) {
    return (ArrayListIterator) {
        .list = list,
        .index = 0
    };
}

void *al_iterator_next(ArrayListIterator *iterator) {
    void *elem = al_get(iterator->list, iterator->index);
    iterator->index++;
    return elem;
}