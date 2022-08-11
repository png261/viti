#include "buffer.h"
#include "mess.h"
#include "util.h"
#include <ncurses.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef MESS_H_INIT
#define MESS_H_INIT
struct Status status;
#endif

extern Buffer *cbuf;

void mess_send(const char *format, ...) {
    wclear(status.win);
    va_list ap;
    va_start(ap, format);
    vsnprintf(status.content, sizeof(status.content), format, ap);
    va_end(ap);
    wprintw(status.win, status.content);
    wrefresh(status.win);
    touchwin(cbuf->win);
}

char *prompt(char *format) {
    char *str = malloc(128 * sizeof(char));
    size_t strlen = 0;
    str[strlen] = '\0';
    while (1) {
        mess_send(format, str);
        int c = getch();
        if (c == '\n') {
            mess_send("");
            return str;
            break;
        }
        if (c == '\x1b') {
            mess_send("");
            break;
        }
        if (c == CTRL('h') || c == KEY_BACKSPACE) {
            str[--strlen] = '\0';
            continue;
        }
        str[strlen++] = c;
        str[strlen] = '\0';
    }
    return str;
}
