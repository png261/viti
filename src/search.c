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
int match_index = 0;
int match_count = 0;
Pos *match_list = NULL;
char *search_query = NULL;

/* local var */
Pos *match_list_old;
char *search_query_old;

void search_callback(char *query, int c) {
    if (!strlen(query)) {
        win_render_rows(cwin);
        return;
    }
    if (c == '\x1b') {
        free(search_query);
        free(match_list);

        match_list = match_list_old;
        search_query = search_query_old;
        win_render_rows(cwin);
        return;
    } else if (c == '\n') {
        /* free(search_query_old); */
        /* free(match_list_old); */
        return;
    }

    search(cwin, query);
}

void searchMode() {
    match_list_old = match_list;
    search_query_old = search_query;

    is_highlight = 1;
    prompt("/%s", search_callback);
    mode_switch(NORMAL);
}

void search_move() {
    Pos *matched = &match_list[match_index];
    if (match_count == 0 || matched == NULL) {
        return;
    }

    mess_send("/%s [%d/%d]", search_query, match_index, match_count);
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
    match_index = 0;


    Buffer *buf = win->buf;

    /* count the pos  */
    match_count = search_count(win, query);
    if(match_count == 0) { 
        win_render_rows(cwin);
        return;
    }

    /* find all pos */
    const int len = strlen(query);
    match_list = xrealloc(match_list, match_count * sizeof(*match_list));
    Pos *pos = match_list;
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
    if (match_list == NULL || match_count == 0) {
        return;
    }

    match_index = (match_index + 1) % match_count;
    search_move();
}

void search_prev() {
    if (match_list == NULL || match_count == 0) {
        return;
    }

    match_index = abs(match_count + match_index - 1) % match_count;
    search_move();
}
