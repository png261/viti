#include "highlight.h"

#include "color.h"
#include "util.h"

#include <string.h>

int is_highlight = true;

void highlight_row(Win *win, int line, const char *query, int color_pair) {
    if (!strlen(query)) {
        return;
    }

    Row *row = &win->buf->rows[line];
    const int len = strlen(query);
    char *match = row->content;
    while ((match = strstr(match, query)) != NULL) {
        mvwchgat(win->textarea, line - win->view.yoff, match - row->content, len, A_NORMAL, color_pair, NULL);
        match += len;
    }
}
