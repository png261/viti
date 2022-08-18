#include "highlight.h"

#include "color.h"
#include "util.h"
#include <string.h>

void highlight_row(Win *win, int line, char *query, int color_pair) {
    Row *row = &win->buf->rows[line];
    int len = strlen(query);
    int from;
    char *match = row->content;
    while ((match = strstr(match, query)) != NULL) {
        from = match - row->content;
        for (int x = from; x < from + len; x++) {
            wattron(win->textarea, COLOR_PAIR(color_pair));
            mvwaddch(win->textarea, line - win->view.yoff, x, row->content[x]);
            wattroff(win->textarea, COLOR_PAIR(color_pair));
        }
        match += len;
    }
}
