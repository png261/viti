#include "highlight.h"

#include "color.h"
#include "util.h"

#include <string.h>


int is_highlight = true;

void highlight_line(WINDOW *win, char * line, const char *query, int color_pair, int y) 
{
    if(line == NULL || strlen(query) == 0) {
        return;
    }

    const int len = strlen(query);
    char *match = line;
    while ((match = strstr(match, query)) != NULL) {
        mvwchgat(win, y, match - line, len, A_NORMAL, color_pair, NULL);
        match += len;
    }
}
