// memory.c:  dealing with allocation and dellocation of memory

#include "memory.h"

#include "util.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void *xrealloc(void *p, const size_t len) {
    p = realloc(p, len);
    if (p == NULL) {
        viti_die("realloc: %s\n", strerror(errno));
    }
    return p;
}

void *xcalloc(const size_t n, const size_t len) {
    void *p = calloc(n, len);
    if (p == NULL) {
        viti_die("calloc: %s\n", strerror(errno));
    }
    return p;
}

void *xmalloc(const size_t len) {
    void *p = malloc(len);
    if (p == NULL) {
        viti_die("malloc: %s\n", strerror(errno));
    }
    return p;
}
