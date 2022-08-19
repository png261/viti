#ifndef MESS_H
#define MESS_H
#include <ncurses.h>

typedef struct message{
    char content[128];
    WINDOW *win;
} Message;

void mess_send(const char *format, ...);
char *prompt(const char *format, void (*callback)(char *, int));

#endif
