// highlight.c: handle highlight

#include "highlight.h"

#include "color.h"
#include "util.h"

#include <stdbool.h>
#include <string.h>

bool is_highlight = true;

void highlight_line(WINDOW *win, int y, char *line, const char *query,
                    int color_pair) {
    if (line == NULL || strlen(query) == 0) {
        return;
    }

    const int len = strlen(query);
    char *match = line;
    while ((match = strstr(match, query)) != NULL) {
        // change color attribute of text existed on screen
        int x = match - line;
        mvwchgat(win, y, x, len, A_NORMAL, color_pair, NULL);
        match += len;
    }
}
