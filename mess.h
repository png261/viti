#ifndef MESS_H
#define MESS_H
#include <ncurses.h>

struct Status{
    char content[128];
    WINDOW *win;
};

void mess_send(const char *format, ...);
char *prompt(char *format);

#endif
