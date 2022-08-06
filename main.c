#include "util.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"
#include "cursor.h"
#include "file_io.h"

Buffer *Bufs;
Buffer *cbuf;

struct {
    char content[128];
    WINDOW *win;
} status;

struct {
    int mode;
} term;

enum termmode {
    NORMAL,
    INSERT,
    COMMAND,
    SEARCH,
    PROMPT,
};

/* prototypes */
void switchMode(int mode);

/* status */
void mess_send(const char *format, ...) {
    wclear(status.win);
    va_list ap;
    va_start(ap, format);
    vsnprintf(status.content, sizeof(status.content), format, ap);
    va_end(ap);
    mvwprintw(status.win, 0, 0, status.content);
    wrefresh(status.win);
}

char *prompt(char *format) {
    char *str = malloc(128 * sizeof(char));
    size_t strlen = 0;
    str[strlen] = '\0';

    while (1) {
        mess_send(format, str);
        char c = getch();
        if (c == '\n') {
            mess_send("");
            break;
        }

        str[strlen++] = c;
        str[strlen] = '\0';
    }

    return str;
}

/* MODE */
void handleScroll(char c) {
    Cursor *cur = &cbuf->cur;

    switch (c) {
    case 'h':
        cur->x--;
        break;
    case 'l':
        cur->x++;
        break;
    case 'j':
        cur->y++;
        if (cur->y >= cbuf->view.y) {
            cbuf->view.yoff += cur->y - cbuf->view.y;
            cbuf->view.yoff = MIN(cbuf->view.yoff, cbuf->file.lines);
            buffer_render_rows(cbuf);
            cur->y = cbuf->view.y;
        }
        break;
    case 'k':
        cur->y--;
        break;
    }
    wmove(cbuf->win, cur->y, cur->x);
    buffer_render_numbercol(cbuf);
}

void normalMode(char c) {
    switch (c) {
    case 'h':
    case 'l':
    case 'j':
    case 'k':
        handleScroll(c);
        break;
    case 'i':
        switchMode(INSERT);
        break;
    case ':':
        switchMode(COMMAND);
        break;
    }
}

void insertMode(char c) {
    switch (c) {
    case '\x1b':
        switchMode(NORMAL);
        break;
    }
}

void commandMode() {
    char *query = prompt(":%s");
    if (*query == 'q') {
        quit();
    }
    switchMode(NORMAL);
}

void loopKey(int mode, void (*callback)(char)) {
    while (1) {
        char c = wgetch(cbuf->win);
        callback(c);
        if (term.mode != mode)
            break;
    }
}

void switchMode(int mode) {
    term.mode = mode;
    switch (term.mode) {
    case NORMAL:
        loopKey(NORMAL, normalMode);
        break;
    case INSERT:
        loopKey(INSERT, insertMode);
        break;
    case COMMAND:
        commandMode();
        break;
    }
}

/* setup */
void ncursesSetup() {
    initscr();
    start_color();
    raw();
    keypad(stdscr, TRUE);
    noecho();
}

void init() {
    term.mode = NORMAL;

    int mess_height = 1;
    Bufs = malloc(sizeof(Buffer));
    Bufs[0] = buffer_create(LINES - mess_height, COLS, 0, 0);
    cbuf = &Bufs[0];

    status.win = newwin(mess_height, COLS, LINES - mess_height, 0);
    refresh();
}

/* main */
int main(int argc, char *argv[]) {
    ncursesSetup();
    init();

    if (argc >= 2) {
        file_open(argv[1], cbuf);
    }

    buffer_render(cbuf);
    cursor_refresh(cbuf);
    switchMode(NORMAL);
    quit();
}
