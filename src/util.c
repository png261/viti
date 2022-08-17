#include <ncurses.h>
#include <stdlib.h>

void die(const char *message) {
    clear();
    endwin();
    perror(message);
    exit(1);
}

void quit() {
    clear();
    endwin();
    exit(0);
}
