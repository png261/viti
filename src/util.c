#include "util.h"

#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void die(const char *errstr, ...) {
    clear();
    endwin();

    va_list ap;

    va_start(ap, errstr);
    vfprintf(stderr, errstr, ap);
    va_end(ap);
    exit(1);
}

void quit() {
    clear();
    endwin();
    exit(0);
}

void *xrealloc(void *p, size_t len) {
    if (!(p = realloc(p, len))) {
        die("realloc: %s\n", strerror(errno));
    }

    return p;
}

void *xcalloc(size_t n, size_t len) {
    void *p;

    if (!(p = calloc(n, len))) {
        die("calloc: %s\n", strerror(errno));
    }

    return p;
}

void *xmalloc(size_t len) {
    void *p;

    if (!(p = malloc(len))) {
        die("malloc: %s\n", strerror(errno));
    }

    return p;
}
