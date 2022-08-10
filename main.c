#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "file_io.h"
#include "mess.h"
#include "util.h"

extern struct Status status;
extern Buffer *cbuf;
extern Buffer *Bufs;

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

/* MODE */
void normalMode(int c) {
    switch (c) {
    case 'h':
        cursor_left(cbuf);
        break;
    case 'l':
        cursor_right(cbuf);
        break;
    case 'j':
        cursor_down(cbuf);
        break;
    case 'k':
        cursor_up(cbuf);
        break;
    case 'i':
        switchMode(INSERT);
        break;
    case ':':
        switchMode(COMMAND);
        break;
    }
}

void insertMode(int c) {
    switch (c) {
    case '\x1b':
        switchMode(NORMAL);
        break;
    case '\n':
        add_line(cbuf->view.line);
        break;
    case KEY_BACKSPACE:
        del_char(cbuf->view.line, cbuf->view.col - 1);
        break;
    default:
        append_char(cbuf->view.line, cbuf->view.col, c);
        break;
    }
}

void commandMode() {
    char *query = prompt(":%s");
    switch (*query) {
    case 'q':
        quit();
        break;
    case 'w':
        file_save(cbuf);
        mess_send("saved");
        break;
    }
    switchMode(NORMAL);
}

void loopKey(int mode, void (*callback)(int)) {
    while (1) {
        int c = wgetch(cbuf->win);
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
