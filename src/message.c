// message.c: handle messsage, prompt on command line

#include "message.h"

#include "key.h"
#include "memory.h"
#include "util.h"
#include "window.h"

#include <ncurses.h>
#include <stdarg.h>
#include <stdlib.h>

Message mess;

extern Win *cwin;

void mess_resize() {
    wresize(mess.win, 1, COLS);
    mvwin(mess.win, LINES - 1, 0);
    wrefresh(mess.win);
    refresh();
}

void mess_send(const char *format, ...) {
    wclear(mess.win);
    va_list ap;
    va_start(ap, format);
    vsnprintf(mess.content, sizeof(mess.content), format, ap);
    va_end(ap);

    waddstr(mess.win, mess.content);
    wrefresh(mess.win);
    touchwin(cwin->wtext);
}

char *prompt(const char *format, void (*callback)(const char *, int)) {
    size_t size = 128;
    char *str = xmalloc(size);
    size_t strlen = 0;
    str[strlen] = '\0';
    while (1) {
        mess_send(format, str);
        int c = getch();
        if (c == '\n') {
            mess_send("");
            if (callback) {
                callback(str, c);
            }
            return str;
        }
        if (c == ESC) {
            mess_send("");
            if (callback) {
                callback(str, c);
            }
            free(str);
            return NULL;
        }

        if (c == CTRL('h') || c == KEY_BACKSPACE) {
            if (strlen != 0) {
                str[--strlen] = '\0';
            }
        } else {
            if (strlen == size - 1) {
                size *= 2;
                str = xrealloc(str, size);
            }

            str[strlen++] = c;
            str[strlen] = '\0';
        }

        if (callback) {
            callback(str, c);
        }
    }
}
