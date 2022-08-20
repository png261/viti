#include "memory.h"

#include "util.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>


void *xrealloc(void *p, const size_t len) 
{
    if (!(p = realloc(p, len))) {
        die("realloc: %s\n", strerror(errno));
    }

    return p;
}

void *xcalloc(size_t n, const size_t len) 
{
    void *p;

    if (!(p = calloc(n, len))) {
        die("calloc: %s\n", strerror(errno));
    }

    return p;
}

void *xmalloc(const size_t len) 
{
    void *p;

    if (!(p = malloc(len))) {
        die("malloc: %s\n", strerror(errno));
    }

    return p;
}
