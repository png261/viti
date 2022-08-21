#include "search.h"

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "util.h"
#include "memory.h"
#include "highlight.h"
#include "message.h"
#include "mode.h"
#include "window.h"

#include <stdlib.h>
#include <string.h>


/* extern */
extern Win *curwin;
extern int is_highlight;

/* export var */
static int match_index = 0;
static int match_count = 0;
Pos *match_list = NULL;
char *search_query = NULL;

/* local var */
Pos *match_list_old;
char *search_query_old;

/* TODO free */
void search_callback(const char *query, const int c) 
{
    if (strlen(query) == 0) {
        win_render_rows(curwin);
        return;
    }
    if (c == '\x1b') {
        match_list = match_list_old;
        search_query = search_query_old;
        win_render_rows(curwin);
        return;
    } else if (c == '\n') {
        search_query_old = NULL;
        match_list_old = NULL;
        return;
    }

    search(curwin, query);
}

void search_mode() 
{
    match_list_old = match_list;
    search_query_old = search_query;

    is_highlight = 1;
    prompt("/%s", search_callback);
    mode_switch(MODE_NORMAL);
}

void search_move() 
{
    Pos *matched = &match_list[match_index];
    if (match_count == 0 || matched == NULL) {
        return;
    }

    mess_send("/%s [%d/%d]", search_query, match_index, match_count);
    curwin->buf->col = matched->x;
    curwin->buf->line = matched->y;
    win_scroll(curwin);
    cursor_refresh(curwin);
}

int  search_count(Win * win, const char * query)
{
    Buffer *buf = win->buf;
    int count = 0;
    for (int y = 0; y < buf->file.lines; y++) {
        Row *row = &buf->rows[y];
        if(row == NULL) {
           break; 
        }

        char *match = row->content;
        while ((match = strstr(match, query)) != NULL) {
            count++;
            match += strlen(query);
        }
    }

    return count; 
}

void search(Win *win, const char *query) 
{
    if (strlen(query) == 0) {
        return;
    }

    search_query = query;
    match_index = 0;

    Buffer *buf = win->buf;

    /* count the pos  */
    match_count = search_count(win, query);
    if(match_count == 0) { 
        win_render_rows(curwin);
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
    win_render_rows(curwin);
    search_move();
}

void search_next() 
{
    if (match_list == NULL || match_count == 0) {
        return;
    }

    match_index = (match_index + 1) % match_count;
    search_move();
}

void search_prev() 
{
    if (match_list == NULL || match_count == 0) {
        return;
    }

    match_index = abs(match_count + match_index - 1) % match_count;
    search_move();
}
