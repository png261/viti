#include "memory.h"

#include "util.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>


void *xrealloc(void *p, const size_t len) 
{
    p = realloc(p, len);
    if (p == NULL) {
        die("realloc: %s\n", strerror(errno));
    }
    return p;
}


void *xcalloc(const size_t n, const size_t len) 
{
    void *p = calloc(n, len);
    if (p == NULL) {
        die("calloc: %s\n", strerror(errno));
    }
    return p;
}


void *xmalloc(const size_t len) 
{
    void *p = malloc(len);
    if (p == NULL) {
        die("malloc: %s\n", strerror(errno));
    }
    return p;
}
