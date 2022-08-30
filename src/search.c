// search.c: handle search function

#include "search.h"

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "highlight.h"
#include "key.h"
#include "memory.h"
#include "message.h"
#include "mode.h"
#include "util.h"
#include "window.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

char *search_query = NULL;

extern Win *curwin;
extern Buffer *curbuf;
extern bool is_highlight;

static int match_index = 0;
static int match_count = 0;
static Pos *match_list = NULL;
static Pos *match_list_old = NULL;
static char *search_query_old = NULL;

static void search_callback(const char *query, const int c) {
    if (strlen(query) == 0) {
        win_render_lines(curwin);
        return;
    }
    if (c == ESC) {
        match_list = match_list_old;
        search_query = search_query_old;
        win_render_lines(curwin);
        return;
    } else if (c == '\n') {
        search_query_old = NULL;
        match_list_old = NULL;
        return;
    }

    search(query);
}

static void search_move() {
    Pos *matched = &match_list[match_index];
    if (matched == NULL) {
        return;
    }

    curbuf->icol = matched->x;
    curbuf->iline = matched->y;
    curbuf->curline = matched->line;
    win_scroll(curwin);
    cursor_refresh(curwin);
}

static int search_count(const char *query) {
    int count = 0;
    Line *line = curbuf->head;
    while (line != NULL) {
        char *match = line->content;
        while ((match = strstr(match, query)) != NULL) {
            count++;
            match += strlen(query);
        }
        line = line->next;
    }
    return count;
}

void search_mode() {
    match_list_old = match_list;
    search_query_old = search_query;
    is_highlight = true;
    prompt("/%s", search_callback);
    mode_switch(MODE_NORMAL);
}

void search(const char *query) {
    if (strlen(query) == 0) {
        return;
    }

    search_query = (char *)query;
    match_index = 0;

    /* count */
    match_count = search_count(query);
    if (match_count == 0) {
        win_render_lines(curwin);
        return;
    }

    /* find all */
    const int len = strlen(query);
    match_list = xrealloc(match_list, match_count * sizeof(*match_list));
    int y = 0;
    Pos *pos = match_list;
    char *match;
    Line *line = curbuf->head;
    while (line != NULL) {
        match = line->content;
        while ((match = strstr(match, query)) != NULL) {
            pos->y = y;
            pos->x = match - line->content;
            pos->line = line;
            pos++;
            match += len;
        }
        y++;
        line = line->next;
    }
    win_render_lines(curwin);
    search_move();
}

void search_next() {
    if (match_count == 0) {
        return;
    }
    match_index = (match_index + 1) % match_count;
    search_move();
}

void search_prev() {
    if (match_count == 0) {
        return;
    }
    match_index = abs(match_count + match_index - 1) % match_count;
    search_move();
}
