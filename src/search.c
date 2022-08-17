#include "search.h"

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "mess.h"
#include "mode.h"
#include "window.h"
#include <stdlib.h>
#include <string.h>

extern Win *cwin;
MatchedList *matched_list;

void search_callback(char *query, int c) { search(cwin, query); }

void searchMode() {
    char *query = prompt("/%s", search_callback);
    /* if (query == NULL) { */
    /*     mode_switch(NORMAL); */
    /*     return; */
    /* } */

    /* search(cwin, query); */
    mode_switch(NORMAL);
}

void search(Win *win, char *query) {
    if (query == NULL) {
        return;
    }

    free(matched_list);
    Matched *matched;
    if (matched_list != NULL) {
        matched = matched_list->matched;
    }

    while (matched != NULL) {
        free(matched);
        matched++;
    }

    matched_list = malloc(sizeof(MatchedList));
    matched_list->len = strlen(query);
    matched_list->matched = malloc(sizeof(Matched));
    matched = matched_list->matched;

    Buffer *buf = win->buf;
    matched_list->n = 0;
    for (int y = 0; y < buf->file.lines; y++) {
        Row *row = buf->rows + y;
        char *match = strstr(row->content, query);
        if (match) {
            matched_list->n++;
            realloc(matched_list->matched,
                    (matched_list->n + 1) * sizeof(Matched));
            matched->x = match - row->content;
            matched->y = y;
            matched++;
        }
    }

    win_render_rows(cwin);
    cursor_refresh(cwin);
}
