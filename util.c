#include <ncurses.h>
#include <stdlib.h>

void die(const char *s) {
    endwin();
    perror(s);
    exit(1);
}

void quit() {
    endwin();
    exit(0);
}
