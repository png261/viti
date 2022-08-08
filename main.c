#include "util.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"
#include "cursor.h"
#include "file_io.h"

Buffer * cbuf;
Buffer * Bufs;

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
    wprintw(status.win, status.content);
    wrefresh(status.win);
    touchwin(cbuf -> win);
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
void normalMode(char c) {
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
    mess_send(cbuf->rows[cbuf->view.line].content);
}

void append(char c) {
    int at = cbuf->view.col;
    Row *row = &cbuf->rows[cbuf->view.line];
    row->content = realloc(row->content, row->size + 2);
    memmove(&row->content[at + 1], &row->content[at], row->size - at + 1);
    row->content[at] = c;
    row->size++;
    buffer_render_rows(cbuf);
    cursor_right(cbuf);
}
void insertNewline() {
    int at = cbuf->view.line;
    cbuf->rows = realloc(cbuf->rows, (cbuf->file.lines + 1) * sizeof(Row));
    memmove(&cbuf->rows[at + 1], &cbuf->rows[at],
            (cbuf->file.lines - at) * sizeof(Row));

    cbuf->rows[at].content[0] = '\0';
    cbuf->file.lines++;

    buffer_render_rows(cbuf);
    cursor_refresh(cbuf);
}

void insertMode(char c) {
    switch (c) {
    case '\x1b':
        switchMode(NORMAL);
        break;
    /* case '\n': */
    /*     insertNewline(); */
    default:
        append(c);
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
