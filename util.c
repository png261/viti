#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h" 

void clearScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void die(const char *s) {
    perror(s);
    exit(1);
    clearScreen();
}
