#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "cursor.h"

Cursor cur;
Buffer buf;

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
void die(char * mess);
void switchMode(int mode);

/* status */
void mess_send(const char *format, ...) {
    wclear(status.win);
    va_list ap;
    va_start(ap, format);
    vsnprintf(status.content, sizeof(status.content), format, ap);
    va_end(ap);
    mvwprintw(status.win, 1, 0, status.content);
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
        cursor_right(&cur);
        break;
    case 'j':
        cur.y++;
        if (cur.y > buf.view.y) {
            buf.view.yoff += cur.y - buf.view.y;
            if (buf.view.yoff > buf.nlines) {
                buf.view.yoff = buf.nlines;
            }
            cur.y = buf.view.y;
        }
        buffer_render(&buf);
        wmove(buf.win, cur.y, cur.x);
        wrefresh(buf.win);
        break;
    case 'k':
        cur.y--;
        if (cur.y < 0) {
            buf.view.yoff += cur.y;
            if (buf.view.yoff < 0) {
                buf.view.yoff = 0;
            }
            cur.y = 0;
        }
        buffer_render(&buf);
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
    if(!fp){
        die("File not found!");
    }
    buf->nlines = getLines(filename);
    buf->rows = calloc(buf->nlines, sizeof(row));

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    int i = 0;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        buf->rows[i].content = malloc(linelen * sizeof(char));
        memcpy(buf->rows[i].content, line, linelen);
        buf->rows[i].size = linelen;
        i++;
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

    buf.view.xoff = 0;
    buf.view.yoff = 0;
    buf.nlines = 0;

    buf.view.x = term.x;
    buf.view.y = term.y - 2;
    buf.win = newwin(buf.view.y, buf.view.x, 0, 0);
    term.wcurrent = buf.win;

    status.win = newwin(2, term.x, term.y - 2, 0);
    refresh();

}

void quit() {
    endwin();
    exit(0);
}

void die(char * mess){
    endwin();
    puts(mess);
    exit(1);
}

/* main */
int main(int argc, char *argv[]) {
    ncursesSetup();
    init();

    if (argc == 2) {
        openFile(&buf, argv[1]);
    }

    buffer_render(&buf);
    cursor_refresh(&cur);
    switchMode(NORMAL);

    quit();
}
