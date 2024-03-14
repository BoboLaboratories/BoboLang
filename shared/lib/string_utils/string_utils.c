#include <malloc.h>
#include <string.h>

#include "lib/string_utils/string_utils.h"

char *strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
}