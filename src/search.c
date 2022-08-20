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
        /* free(search_query_old); */
        /* free(matched_list_old); */
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
    Pos *matched = &matched_list[matched_index];
    if (matched_count == 0 || matched == NULL) {
        return;
    }

    mess_send("/%s [%d/%d]", search_query, matched_index, matched_count);
    cwin->buf->col = matched->x;
    cwin->buf->line = matched->y;
    win_scroll(cwin);
    cursor_refresh(cwin);
}

int  search_count(Win * win, const char * query){
    Buffer *buf = win->buf;
    const int len = strlen(query);
    int count = 0;
    for (int y = 0; y < buf->file.lines; y++) {
        Row *row = &buf->rows[y];
        char *match = row->content;

        while ((match = strstr(match, query)) != NULL) {
            count++;
            match += len;
        }
    }

    return count; 

}

void search(Win *win, const char *query) {
    if (!strlen(query)) {
        return;
    }

    search_query = query;
    matched_index = 0;


    Buffer *buf = win->buf;

    /* count the pos  */
    matched_count = search_count(win, query);
    if(matched_count == 0) { 
        win_render_rows(cwin);
        return;
    }

    /* find all pos */
    const int len = strlen(query);
    matched_list = xrealloc(matched_list, matched_count * sizeof(*matched_list));
    Pos *pos = matched_list;
    for (int y = 0; y < buf->file.lines; y++) {
        Row *row = &buf->rows[y];
        char *match = row->content;

        while ((match = strstr(match, query)) != NULL) {
            pos->x = match - row->content;
            pos->y = y;
            pos++;
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
