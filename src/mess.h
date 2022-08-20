#ifndef VIIT_MESS_H
#define VITI_MESS_H

#include <ncurses.h>

typedef struct message{
    char content[128];
    WINDOW *win;
} Message;

void mess_send(const char *format, ...);
char *prompt(const char *format, void (*callback)(const char *, int));

#endif
