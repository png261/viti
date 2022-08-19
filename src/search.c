#include "search.h"

#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "util.h"
#include "highlight.h"
#include "mess.h"
#include "mode.h"
#include "window.h"

/* extern */
extern Win *cwin;
extern int is_highlight;

/* export var */
int matched_index = 0;
int matched_count = 0;
Pos *matched_list = NULL;
char *search_query = NULL;

/* local var */
Pos *matched_list_old;
char *search_query_old;

void search_callback(char *query, int c) {
    if (!strlen(query)) {
        win_render_rows(cwin);
        return;
    }
    if (c == '\x1b') {
        /* free(search_query); */
        /* free(matched_list); */

        matched_list = matched_list_old;
        search_query = search_query_old;
        win_render_rows(cwin);
        return;
    } else if (c == '\n') {
        /* free(matched_list_old); */
        /* free(search_query_old); */
        return;
    }

    search(cwin, query);
}

void searchMode() {
    matched_list_old = matched_list;
    search_query_old = search_query;

    is_highlight = 1;
    prompt("/%s", search_callback);
    mode_switch(NORMAL);
}

void search_move() {
    if (!matched_count) {
        return;
    }
    Pos *matched = &matched_list[matched_index];
    cwin->buf->col = matched->x;
    cwin->buf->line = matched->y;
    handleScroll(cwin);
    cursor_refresh(cwin);
}

void search(Win *win, char *query) {
    if (!strlen(query)) {
        return;
    }

    search_query = query;
    matched_index = 0;

    matched_list = xmalloc(sizeof(*matched_list));
    Pos *matched = matched_list;

    Buffer *buf = win->buf;
    matched_count = 0;
    const int len = strlen(query);

    /* find all postions */
    for (int y = 0; y < buf->file.lines; y++) {
        Row *row = &buf->rows[y];
        char *match = row -> content;
        while ((match = strstr(match, query)) != NULL) {
            matched->x = match - row->content;
            matched->y = y;
            matched_count++;

            matched_list = xrealloc(matched_list, (matched_count + 1) * sizeof(*matched_list));
            matched++;

            match += len;
        }
    }
    win_render_rows(cwin);
    search_move();
}

void search_next() {
    if (matched_list == NULL || matched_count == 0) {
        return;
    }

    matched_index = (matched_index + 1) % matched_count;
    search_move();
}

void search_prev() {
    if (matched_list == NULL || matched_count == 0) {
        return;
    }

    matched_index = abs(matched_count + matched_index - 1) % matched_count;
    search_move();
}
