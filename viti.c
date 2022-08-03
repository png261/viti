/* includes */
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#define _GNU_SOURCE

#include <ctype.h>

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "util.h"

/* defines */
#define TAB_STOP 4
#define CTRL_KEY(k) ((k)&0x1f)

enum keys {
    KEY_LEFT = 'h',
    KEY_RIGHT = 'l',
    KEY_UP = 'k',
    KEY_DOWN = 'j',
    ESC = '\x1b',
    ENTER = '\r',
    TAB = '\t',
    BACKSPACE = 127,
    DEL_KEY = 1000,
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,

    NORMAL_KEY = ESC,
    INSERT_KEY = 'i',
    VISUAL_KEY = 'v',
    SEARCH_KEY = '/',
    COMAND_KEY = ':',
};

enum MODE { NORMAL, INSERT, VISUAL };

/* data */
struct Cursor {
    int x;
    int y;
    int rx;
} Cur;

typedef struct erow {
    int size;
    int rsize;
    char *chars;
    char *render;
} erow;

struct {
    int rows;
    int cols;
    int rowoff;
    int coloff;
} Win;

struct editor {
    int mode;
    int premode;

    int nrows;
    erow *row;
    char *filename;
    char message[80];
    struct termios orig_termios;
} E;

/* prototypes */
void refreshScreen();
void message(const char *fmt, ...);
char *prompt(char *mess, void (*callback)(char *, int));
void insertKey(int c);
void normalKey(int c);
void visualKey(int c);
void moveCursor(int c);

/* terminal */
void disableRawMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios); }

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &E.orig_termios);
    atexit(disableRawMode);

    struct termios raw = E.orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0.1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int readKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }

    if (c == '\x1b') {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return '\x1b';

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1)
                    return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                    case '3':
                        return DEL_KEY;
                    }
                }
            } else {
                switch (seq[1]) {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
                }
            }
        }
        return '\x1b';
    } else {
        return c;
    }
}

int loadWinSize(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    }

    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
}

/* row operations */
int CxToRx(erow *row, int cx) {
    int rx = 0;
    for (int i = 0; i < cx; i++) {
        if (row->chars[i] == TAB) {
            rx += (TAB_STOP - 1) - (rx % TAB_STOP);
        }
        rx++;
    }
    return rx;
}

int RxToCx(erow *row, int rx) {
    int cur_rx = 0;
    int cx;
    for (cx = 0; cx < row->size; cx++) {
        if (row->chars[cx] == TAB) {
            cur_rx += (TAB_STOP - 1) - (cur_rx % TAB_STOP);
        }
        cur_rx++;
        if (cur_rx > rx) {
            return cx;
        }
    }
    return cx;
}

int countTab(erow *row) {
    int tabs = 0;
    for (int i = 0; i < row->size; i++) {
        if (row->chars[i] == TAB) {
            tabs++;
        }
    }
    return tabs;
}

void updateRow(erow *row) {
    int tabs = countTab(row);

    free(row->render);
    row->render = malloc(row->size + tabs * (TAB_STOP - 1) + 1);

    int idx = 0;
    for (int i = 0; i < row->size; i++) {
        if (row->chars[i] == TAB) {
            row->render[idx++] = ' ';
            while (idx % TAB_STOP != 0) {
                row->render[idx++] = ' ';
            }
        } else {
            row->render[idx++] = row->chars[i];
        }
    }
    row->render[idx] = '\0';
    row->rsize = idx;
}

void insertRow(int at, char *s, size_t len) {
    if (at < 0 || at > E.nrows) {
        return;
    }

    E.row = realloc(E.row, sizeof(erow) * (E.nrows + 1));
    memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.nrows - at));

    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';
    E.row[at].rsize = 0;
    E.row[at].render = NULL;
    updateRow(&E.row[at]);
    E.nrows++;
}

void freeRow(erow *row) {
    free(row->render);
    free(row->chars);
}

void delRow(int at) {
    if (at < 0 || at >= E.nrows) {
        return;
    }

    freeRow(&E.row[at]);
    memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.nrows - at - 1));
    E.nrows--;
}

void rowInsertChar(erow *row, int at, int c) {
    if (at < 0 || at > row->size) {
        at = row->size;
    }
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = c;
    updateRow(row);
}

void rowAppendString(erow *row, char *s, size_t len) {
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    updateRow(row);
}

void rowDelChar(erow *row, int at) {
    if (at < 0 || at >= row->size) {
        return;
    }

    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    updateRow(row);
}

/* editor operations */
void insertChar(char c) {
    if (Cur.y == E.nrows) {
        insertRow(E.nrows, "", 0);
    }
    rowInsertChar(&E.row[Cur.y], Cur.x, c);
    Cur.x++;
}

void insertNewline() {
    if (Cur.x == 0) {
        insertRow(Cur.y, "", 0);
    } else {
        erow *row = &E.row[Cur.y];
        insertRow(Cur.y + 1, &row->chars[Cur.x], row->size - Cur.x);
        row = &E.row[Cur.y];
        row->size = Cur.x;
        row->chars[row->size] = '\0';
        updateRow(row);
    }
    Cur.y++;
    Cur.x = 0;
}

void delChar() {
    if ((Cur.y == E.nrows) | (Cur.x == 0 && Cur.y == 0)) {
        return;
    }

    erow *row = &E.row[Cur.y];
    if (Cur.x > 0) {
        rowDelChar(row, Cur.x - 1);
        Cur.x--;
        return;
    }

    Cur.x = E.row[Cur.y - 1].size;
    rowAppendString(&E.row[Cur.y - 1], row->chars, row->size);
    delRow(Cur.y);
    Cur.y--;
}

/* file i/o */
char *RowsToString(int *buflen) {
    int totlen = 0;
    for (int i = 0; i < E.nrows; i++) {
        totlen += E.row[i].size + 1;
    }
    *buflen = totlen;

    char *buf = malloc(totlen);
    char *p = buf;
    for (int i = 0; i < E.nrows; i++) {
        memcpy(p, E.row[i].chars, E.row[i].size);
        p += E.row[i].size;
        *p = '\n';
        p++;
    }

    return buf;
}

void openFile(char *filename) {
    FILE *fp = fopen(filename, "r");
    E.filename = strdup(filename);

    if (!fp) {
        return;
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while ((line[linelen - 1] == '\n' || line[linelen - 1] == '\r')) {
            linelen--;
        }
        insertRow(E.nrows, line, linelen);
    }
    free(line);
    fclose(fp);
}

void saveFile() {
    if (E.filename == NULL) {
        message("\033[33mNo file name\033[m");
        return;
    }

    int len;
    char *buf = RowsToString(&len);
    int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
    ftruncate(fd, len);
    write(fd, buf, len);
    message("\"%s\" %dL, %dB written", E.filename, E.nrows, len);
    close(fd);
    free(buf);
}

/* find */
void findCallback(char *query, int key) {
    if (key == ENTER || key == ESC) {
        return;
    }

    for (int i = 0; i < E.nrows; i++) {
        erow *row = &E.row[i];
        char *match = strstr(row->render, query);
        if (!match) {
            continue;
        }

        Cur.y = i;
        Cur.x = RxToCx(row, match - row->render);
        Win.rowoff = E.nrows;
    }
}

void find() {
    int saved_cx = Cur.x;
    int saved_cy = Cur.y;
    int saved_coloff = Win.coloff;
    int saved_rowoff = Win.rowoff;

    char *query = prompt("/%s", findCallback);
    if (query) {
        message("/%s", query);
        free(query);
        return;
    }

    Cur.x = saved_cx;
    Cur.y = saved_cy;
    Win.coloff = saved_coloff;
    Win.rowoff = saved_rowoff;
}

/* append buffer */
struct abuf {
    char *b;
    int len;
};

void append(struct abuf *ab, const char *s, int len) {
    char *new = realloc(ab->b, ab->len + len);
    if (new == NULL) {
        return;
    }

    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void abFree(struct abuf *ab) { free(ab->b); }

/* input */
char *prompt(char *mess, void (*callback)(char *, int)) {
    size_t bufsize = 128;
    char *buf = malloc(bufsize);
    size_t buflen = 0;
    buf[0] = '\0';
    while (1) {
        message(mess, buf);
        refreshScreen();
        int c = readKey();
        if ((c == DEL_KEY || c == BACKSPACE) && buflen != 0) {
            buf[--buflen] = '\0';
        }

        if (c == ESC) {
            message("");
            free(buf);
            return NULL;
        }

        if (c == ENTER && buflen != 0) {
            message("");
            if (callback) {
                callback(buf, c);
            }
            return buf;
        }

        if (!iscntrl(c) && c < 128) {
            if (buflen == bufsize - 1) {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }

        if (callback && buflen != 0) {
            callback(buf, c);
        }
    }
}

void moveCursor(int key) {
    erow *row = (Cur.y >= E.nrows) ? NULL : &E.row[Cur.y];

    switch (key) {
    case KEY_LEFT:
    case ARROW_LEFT:
        if (Cur.x != 0) {
            Cur.x--;
        }
        break;
    case KEY_RIGHT:
    case ARROW_RIGHT:
        if (row && Cur.x < row->size) {
            Cur.x++;
        }
        break;
    case KEY_UP:
    case ARROW_UP:
        if (Cur.y != 0) {
            Cur.y--;
        }
        break;
    case KEY_DOWN:
    case ARROW_DOWN:
        if (Cur.y < E.nrows) {
            Cur.y++;
        }
        break;
    }
}

void handleCommand() {
    char *command = prompt(":%s", NULL);
    if (command == NULL)
        return;

    switch (*command) {
    case 'w':
        saveFile();
        break;
    case 'q':
        clearScreen();
        exit(0);
        break;
    }
}

void normalKey(int c) {
    switch (c) {
    case KEY_UP:
    case KEY_DOWN:
    case KEY_RIGHT:
    case KEY_LEFT:
        moveCursor(c);
        break;
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_RIGHT:
    case ARROW_LEFT:
        moveCursor(c);
        break;
    case INSERT_KEY:
        E.mode = INSERT;
        break;
    case VISUAL_KEY:
        E.mode = VISUAL;
        break;
    case SEARCH_KEY:
        find();
        break;
    case COMAND_KEY:
        handleCommand();
        break;
    case 'x':
        moveCursor(KEY_RIGHT);
        delChar();
        break;
    case 'o':
        moveCursor(KEY_DOWN);
        insertRow(Cur.y, "", 0);
        Cur.x = 0;
        E.mode = INSERT;
        break;
    case 'O':
        insertRow(Cur.y, "", 0);
        Cur.x = 0;
        E.mode = INSERT;
        break;
    case 'd':
        if (readKey() == 'd') {
            delRow(Cur.y);
            Cur.x = RxToCx(&E.row[Cur.y], Cur.x);
        }
        break;
    case 'a':
        moveCursor(KEY_RIGHT);
        E.mode = INSERT;
        break;
    case '0':
        Cur.x = 0;
        break;
    case '$':
        Cur.x = E.row[Cur.y].size;
        break;
    case CTRL_KEY('d'):
        Cur.y += Win.cols - 1;
        if (Cur.y > E.nrows) {
            Cur.y = E.nrows - 1;
        }
        break;
    case CTRL_KEY('u'):
        Cur.y -= Win.cols - 1;
        if (Cur.y < 0) {
            Cur.y = 0;
        }
        break;
    }
}

void insertKey(int c) {
    switch (c) {
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_RIGHT:
    case ARROW_LEFT:
        moveCursor(c);
        break;
    case NORMAL_KEY:
        E.mode = NORMAL;
        break;
    case VISUAL_KEY:
        E.mode = VISUAL;
        break;
    case ENTER:
        insertNewline();
        break;
    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
        if (c == DEL_KEY) {
            moveCursor(KEY_RIGHT);
        }
        delChar();
        break;
    default:
        insertChar(c);
    }
}

void handleKeyPress() {
    int c = readKey();
    switch (E.mode) {
    case NORMAL:
        normalKey(c);
        break;
    case INSERT:
        insertKey(c);
        break;
    case VISUAL:
        /* visualKey(c); */
        break;
    }
}

/* output */

void renderText(struct abuf *ab, int rowIndex) {
    int len = E.row[rowIndex].rsize - Win.coloff;
    if (len < 0) {
        len = 0;
    }
    if (len > Win.cols) {
        len = Win.cols;
    }
    append(ab, &E.row[rowIndex].render[Win.coloff], len);
}

void renderBlank(struct abuf *ab) { append(ab, "~", 1); }

void renderRows(struct abuf *ab) {
    for (int y = 0; y < Win.rows; y++) {
        int filerow = y + Win.rowoff;
        if (filerow >= E.nrows) {
            renderBlank(ab);
        } else {
            renderText(ab, filerow);
        }

        append(ab, "\x1b[K", 3);
        append(ab, "\r\n", 2);
    }
}

void handleScroll() {
    Cur.rx = 0;
    if (Cur.y < E.nrows) {
        Cur.rx = CxToRx(&E.row[Cur.y], Cur.x);
    }
    if (Cur.y < Win.rowoff) {
        Win.rowoff = Cur.y;
    }
    if (Cur.y >= Win.rowoff + Win.rows) {
        Win.rowoff = Cur.y - Win.rows + 1;
    }
    if (Cur.rx < Win.coloff) {
        Win.coloff = Cur.rx;
    }
    if (Cur.rx >= Win.coloff + Win.cols) {
        Win.coloff = Cur.rx - Win.cols + 1;
    }
}

void renderMessage(struct abuf *ab) {
    append(ab, "\x1b[K", 3);
    int len = strlen(E.message);
    if (len > Win.cols) {
        len = Win.cols;
    }
    append(ab, E.message, len);
}

void renderCursor(struct abuf *ab) {
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (Cur.y - Win.rowoff) + 1,
             (Cur.rx - Win.coloff) + 1);
    append(ab, buf, strlen(buf));
}

void refreshScreen() {
    handleScroll();
    struct abuf ab = {NULL, 0};

    append(&ab, "\x1b[?25l", 6);
    append(&ab, "\x1b[H", 3);

    renderRows(&ab);
    renderMessage(&ab);
    renderCursor(&ab);

    append(&ab, "\x1b[?25h", 6);

    write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
}

void message(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.message, sizeof(E.message), fmt, ap);
    va_end(ap);
}

/* setup */
void setup() {
    E.mode = NORMAL;

    E.nrows = 0;
    E.row = NULL;
    E.filename = NULL;

    Win.rowoff = 0;
    Win.coloff = 0;

    Cur.x = 0;
    Cur.y = 0;
    Cur.rx = 0;

    loadWinSize(&Win.rows, &Win.cols);
    Win.rows -= 1;
}

int main(int argc, char *argv[]) {
    enableRawMode();
    setup();
    if (argc == 2) {
        openFile(argv[1]);
    }

    while (1) {
        refreshScreen();
        handleKeyPress();
    }
    return 0;
}
