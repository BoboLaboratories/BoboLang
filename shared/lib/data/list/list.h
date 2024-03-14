#ifndef BOBO_LANG_STRUCTURES_LIST_H
#define BOBO_LANG_STRUCTURES_LIST_H

#include <stdbool.h>

typedef struct list List;

void list_create(List **list, size_t max);
bool list_add(List *list, void *elem);
void *list_get(List *list, int index);
bool list_is_full(List *list);
unsigned long list_size(List *list);
unsigned long list_max_size(List *list);

#endif