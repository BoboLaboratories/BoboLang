/*
 * MIT License
 *
 * Copyright (C) 2024 BoboLabs.net
 * Copyright (C) 2024 Mattia Mignogna (https://stami.bobolabs.net)
 * Copyright (C) 2024 Fabio Nebbia (https://glowy.bobolabs.net)
 * Copyright (C) 2024 Third party contributors
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
    if (index < al_size(list)) {
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