// buffer.c: handle buffer

//
// lines is represented as a double linked-list 
//

#include "buffer.h"

#include "memory.h"
#include "util.h"
#include "window.h"

#include <stdlib.h>
#include <string.h>

Buffer *curbuf;
extern Win *curwin;

Buffer *buffer_create() {
    Buffer *buf = xmalloc(sizeof(*buf));
    buf->head = NULL;
    buf->tail = NULL;
    buf->curline = NULL;
    buf->nlines = 0;

    buf->iline = 0;
    buf->icol = 0;

    buf->name = NULL;
    return buf;
}

static Line *line_new(char *content, size_t size) {
    Line *line = malloc(sizeof(*line));
    line->content = malloc(size * sizeof(*line->content));
    memcpy(line->content, content, size);
    line->size = size;
    line->prev = NULL;
    line->next = NULL;
    return line;
}

void line_push(char *content, size_t size) {
    Line *new = line_new(content, size);
    if (curbuf->nlines == 0) {
        curbuf->head = new;
    } else {
        curbuf->tail->next = new;
        new->prev = curbuf->tail;
    }
    curbuf->tail = new;
    curbuf->nlines++;
}

void line_insert_after(Line *line, char *content, size_t size) {
    Line *new = line_new(content, size);
    new->next = line->next;
    line->next = new;
    new->prev = line;
    if (line == curbuf->tail) {
        curbuf->tail = new;
    } else {
        new->next->prev = new;
    }
    curbuf->nlines++;
}

void line_insert_before(Line *line, char *content, size_t size) {
    Line *new = line_new(content, size);
    new->next = line;
    if (line == curbuf->head) {
        curbuf->head = new;
    } else {
        new->prev = line->prev;
        new->prev->next = new;
    }
    line->prev = new;
    curbuf->nlines++;
}

static void free_line(Line *line) {
    line->size = 0;
    free(line->content);
    free(line);
}

static void line_remove_head() {
    if (curbuf->curline == curbuf->head) {
        curbuf->curline = curbuf->head->next;
    }
    update_top_line(curwin);
    curbuf->head = curbuf->head->next;
    curbuf->head->prev = NULL;
    curbuf->nlines--;
}

static void line_remove_tail() {
    if (curbuf->curline == curbuf->tail) {
        curbuf->curline = curbuf->tail->prev;
    }
    curbuf->tail = curbuf->tail->prev;
    curbuf->tail->next = NULL;
    curbuf->nlines--;
}

static void line_empty(Line *line) {
    free(line->content);
    line->content = NULL;
    line->size = 0;
}

void line_remove(Line *del) {
    if (curbuf->nlines == 1) {
        line_empty(del);
        return;
    }

    if (del == curbuf->head) {
        line_remove_head();
        free(del);
        return;
    }
    if (del == curbuf->tail) {
        line_remove_tail();
        free(del);
        return;
    }
    if (curbuf->curline == del) {
        curbuf->curline = del->next;
    }
    del->prev->next = del->next;
    del->next->prev = del->prev;
    curbuf->nlines--;
    free_line(del);
}

int buffer_progress(Buffer *buf) {
    return (int)((float)(buf->iline + 1) / buf->nlines * 100);
}
