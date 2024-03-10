#include <stdbool.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <math.h>

#include "lib/structures/list.h"

struct list {
    unsigned long max_size;
    unsigned long size;
    void **values;
};

void list_create(List **list, size_t max) {
    *list = malloc(sizeof(List));
    (*list)->max_size = (unsigned long) pow(2, (double) (8 * max)) - 1;
    (*list)->values = NULL;
    (*list)->size = 0;
}

bool list_add(List *list, void *elem) {
    if (list_is_full(list)) {
        return false;
    }

    list->values = reallocarray(list->values, list->size + 1, sizeof(void *));
    if (errno == ENOMEM) {
        return false;
    }

    list->values[list->size] = elem;
    list->size++;
    return true;
}

void *list_get(List *list, int index) {
    void *elem = NULL;
    if (index <= list_size(list)) {
        elem = list->values[index];
    }
    return elem;
}

bool list_is_full(List *list) {
    return list_size(list) >= list_max_size(list);
}

unsigned long list_size(List *list) {
    return list->size;
}

unsigned long list_max_size(List *list) {
    return list->max_size;
}

void list_free(List *list) {
    int i;
    for (i = 0; i < list_size(list); i++) {
        free(list->values[i]);
    }
    free(list);
}