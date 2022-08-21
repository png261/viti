#include "util.h"

#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


void die(const char *errstr, ...) 
{
    clear();
    endwin();

    va_list ap;
    va_start(ap, errstr);
    vfprintf(stderr, errstr, ap);
    va_end(ap);
    exit(1);
}

void quit() 
{
    clear();
    endwin();
    exit(0);
}

