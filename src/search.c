#include "search.h"

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "highlight.h"
#include "mess.h"
#include "mode.h"
#include "window.h"
#include <stdlib.h>
#include <string.h>

extern Win *cwin;
MatchedList *matched_list;
MatchedList *matched_list_old;

int matched_index = 0;
char *search_query;
char *search_query_old;

void search_callback(char *query, int c) {
    search(cwin, query);
    if (c == '\x1b') {
        free(matched_list);
        matched_list = matched_list_old;
        search_query = search_query_old;
        win_render_rows(cwin);
        return;
    } else if (c == '\n') {
        free(matched_list_old);
        free(search_query_old);
        return;
    }
}

void searchMode() {
    matched_list_old = matched_list;
    search_query_old = search_query;
    char *query = prompt("/%s", search_callback);
    if (query == NULL) {
        mode_switch(NORMAL);
        return;
    }
    search_query = query;

    mess_send("/%s", query);
    mode_switch(NORMAL);
}

void search(Win *win, char *query) {
    if (query == NULL) {
        return;
    }

    matched_index = 0;
    matched_list = malloc(sizeof(MatchedList));
    search_query = query;
    matched_list->matched = malloc(sizeof(Matched));
    Matched *matched = matched_list->matched;

    Buffer *buf = win->buf;
    matched_list->n = 0;
    int len = strlen(query);

    /* find all postions */
    for (int y = 0; y < buf->file.lines; y++) {
        Row *row = &buf->rows[y];
        char *str = row->content;
        char *match;
        while ((match = strstr(str, query)) != NULL) {
            matched_list->n++;
            matched->x = match - row->content;
            matched->y = y;
            realloc(matched_list->matched,
                    (matched_list->n + 1) * sizeof(Matched));
            matched++;
            str = match + len;
        }
    }
    win_render_rows(cwin);

    if (matched_list->n == 0) {
        return;
    }

    /* move cursor to the first matched */
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
