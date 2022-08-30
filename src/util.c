#include "util.h"

#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void viti_die(const char *errstr, ...) {
    clear();
    endwin();

    va_list ap;
    va_start(ap, errstr);
    vfprintf(stderr, errstr, ap);
    va_end(ap);
    exit(1);
}

void viti_quit() {
    clear();
    endwin();
    exit(0);
}

size_t trim(char *str) {
    char *c = str + strlen(str) - 1;
    while (*c == '\n' || *c == ' ') {
        *c = '\0';
    }
    return strlen(str);
}
