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
MatchedList *matched_list_old;
static int matched_index = 0;

void search_callback(char *query, int c) {
    search(cwin, query);
    if (c == '\x1b') {
        free(matched_list);
        matched_list = matched_list_old;
        return;
    } else if (c == '\n') {
        free(matched_list_old);
        return;
    }
}

void searchMode() {
    matched_list_old = matched_list;
    char *query = prompt("/%s", search_callback);
    if (query == NULL) {
        mode_switch(NORMAL);
        return;
    }
    matched_list->query = query;

    mess_send("/%s", query);
    mode_switch(NORMAL);
}

void search(Win *win, char *query) {
    if (query == NULL) {
        return;
    }

    Matched *matched;
    if (matched_list != NULL) {
        matched = matched_list->matched;
    }

    matched_index = 0;
    matched_list = malloc(sizeof(MatchedList));
    matched_list->len = strlen(query);
    matched_list->matched = malloc(sizeof(Matched));
    matched = matched_list->matched;

    Buffer *buf = win->buf;
    matched_list->n = 0;
    for (int y = 0; y < buf->file.lines; y++) {
        Row *row = &buf->rows[y];
        char *match = strstr(row->content, query);
        if (match) {
            matched_list->n++;
            matched->x = match - row->content;
            matched->y = y;
            realloc(matched_list->matched,
                    (matched_list->n + 1) * sizeof(Matched));
            matched++;
        }
    }

    win_render_rows(cwin);
    cwin->buf->col = matched_list->matched->x;
    cwin->buf->line = matched_list->matched->y;
    handleScroll(cwin);
    cursor_refresh(cwin);
}

void search_next() {
    if (matched_list == NULL || matched_list->n == 0) {
        return;
    }

    matched_index++;
    Matched *matched =
        matched_list->matched + (matched_index % matched_list->n);
    cwin->buf->col = matched->x;
    cwin->buf->line = matched->y;
    handleScroll(cwin);
    cursor_refresh(cwin);
}

void search_prev() {
    if (matched_list == NULL || matched_list->n == 0) {
        return;
    }

    matched_index--;
    Matched *matched = matched_list->matched +
                       ((matched_list->n + matched_index) % matched_list->n);
    cwin->buf->col = matched->x;
    cwin->buf->line = matched->y;
    handleScroll(cwin);
    cursor_refresh(cwin);
}
