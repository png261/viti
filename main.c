#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "cursor.h"

Cursor cur;
Buffer buf;
Buffer buf2;

struct {
    char content[128];
    WINDOW *win;
} status;

struct {
    int x;
    int y;
    WINDOW *wcurrent;
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
void quit();
void die(char *mess);
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
void normalMode(char c) {
    switch (c) {
    case 'i':
        switchMode(INSERT);
        break;
    case 'h':
        cursor_left(&cur);
        break;
    case 'l':
        cur.x++;

        wmove(buf.win, cur.y, cur.x);
        wrefresh(buf.win);
        break;
    case 'j':
        cur.y++;
        wmove(buf.win, cur.y, cur.x);
        wrefresh(buf.win);
        break;
    case 'k':
        cur.y--;
        wmove(buf.win, cur.y, cur.x);
        wrefresh(buf.win);
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
        char c = wgetch(term.wcurrent);
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

/* file i/o */
int getLines(char *filename) {
    FILE *fp = fopen(filename, "r");
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    int count = 0;
    while ((getline(&line, &linecap, fp)) != -1) {
        count++;
    }

    fclose(fp);
    return count;
}

void openFile(Buffer *buf, char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        die("File not found!");
    }

    buf->filename = filename;
    buf->nlines = getLines(filename);
    buf->rows = calloc(buf->nlines, sizeof(row));

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    row *current = buf->rows;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        current->content = malloc(linelen * sizeof(char));
        memcpy(current->content, line, linelen);
        current->size = linelen;
        current++;
    }
    fclose(fp);
}

/* setup */
void ncursesSetup() {
    initscr();
    raw();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
}

void init() {
    term.mode = NORMAL;
    term.y = LINES;
    term.x = COLS;
    cur.x = 0;
    cur.y = 0;

    int mess_height = 1;
    buf = buffer_create(term.y - mess_height, term.x / 2, 0, 0);
    buf2 = buffer_create(term.y - mess_height, term.x / 2, 0, term.x / 2);
    term.wcurrent = buf.win;

    status.win = newwin(mess_height, term.x, term.y - mess_height, 0);
    refresh();
}

void quit() {
    endwin();
    exit(0);
}

void die(char *mess) {
    endwin();
    puts(mess);
    exit(1);
}

/* main */
int main(int argc, char *argv[]) {
    ncursesSetup();
    init();

    if (argc >= 2) {
        openFile(&buf, argv[1]);
        openFile(&buf2, argv[1]);
    }

    buffer_render(&buf);
    buffer_render(&buf2);

    cursor_refresh(&cur);
    switchMode(NORMAL);

    quit();
}
