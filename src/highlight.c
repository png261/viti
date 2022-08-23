#include "highlight.h"

#include "color.h"
#include "util.h"

#include <string.h>


int is_highlight = true;

void highlight_line(Win *win, Line *line, const char *query, int color_pair) 
{
    if(strlen(query) == 0 || line == NULL || line->size == 0) {
        return;
    }

    const int len = strlen(query);
    char *match = line->content;

    while ((match = strstr(match, query)) != NULL) {
        wattron(win->textarea, COLOR_PAIR(color_pair));
        mvwaddnstr(win->textarea, (line - win->buf->lines) - win->view.yoff, match - line->content, match, len);
        wattroff(win->textarea, COLOR_PAIR(color_pair));
        match += len;
    }
}
