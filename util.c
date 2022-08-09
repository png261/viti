#include <ncurses.h>
#include <stdlib.h>

void die(const char *message) {
    clear();
    endwin();
    perror(message);
    exit(1);
}

void quit() {
    clear();
    endwin();
    exit(0);
}

void elog(const char *format, ...) {
    WINDOW *wlog = newwin(4, 30, 0, COLS - 30);
    refresh();

    char content[128];
    va_list ap;
    va_start(ap, format);
    vsnprintf(content, sizeof(content), format, ap);
    va_end(ap);

    box(wlog, 0, 0);
    mvwaddstr(wlog, 1, 1, content);
    wrefresh(wlog);
    delwin(wlog);
}
