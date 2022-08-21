#include "message.h"

#include "util.h"
#include "window.h"
#include "memory.h"

#include <ncurses.h>
#include <stdarg.h>
#include <stdlib.h>


Message mess;

extern Win *curwin;

void mess_send(const char *format, ...) 
{
    wclear(mess.win);
    va_list ap;
    va_start(ap, format);
    vsnprintf(mess.content, sizeof(mess.content), format, ap);
    va_end(ap);

    waddstr(mess.win, mess.content);
    wrefresh(mess.win);
    touchwin(curwin->textarea);
}

char *prompt(const char *format, void (*callback)(const char *, int)) 
{
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
        if (c == '\x1b') {
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
