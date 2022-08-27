#ifndef VITI_MESSAGE_H
#define VITI_MESSAGE_H

#include <ncurses.h>


typedef struct message{
    char content[128];
    WINDOW *win;
} Message;


void mess_resize();
void mess_send(const char *format, ...);
char *prompt(const char *format, void (*callback)(const char *, int));

#endif // VITI_MESSAGE_H
