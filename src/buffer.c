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

Buffer *cbuf;
extern Win *cwin;

Buffer *buffer_create() {
    Buffer *buf = xmalloc(sizeof(*buf));
    buf->head = NULL;
    buf->tail = NULL;
    buf->cline = NULL;
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
    if (cbuf->nlines == 0) {
        cbuf->head = new;
    } else {
        cbuf->tail->next = new;
        new->prev = cbuf->tail;
    }
    cbuf->tail = new;
    cbuf->nlines++;
}

void line_insert_after(Line *line, char *content, size_t size) {
    Line *new = line_new(content, size);
    new->next = line->next;
    line->next = new;
    new->prev = line;
    if (line == cbuf->tail) {
        cbuf->tail = new;
    } else {
        new->next->prev = new;
    }
    cbuf->nlines++;
}

void line_insert_before(Line *line, char *content, size_t size) {
    Line *new = line_new(content, size);
    new->next = line;
    if (line == cbuf->head) {
        cbuf->head = new;
    } else {
        new->prev = line->prev;
        new->prev->next = new;
    }
    line->prev = new;
    cbuf->nlines++;
}

static void free_line(Line *line) {
    line->size = 0;
    free(line->content);
    free(line);
}

static void line_remove_head() {
    if (cbuf->cline == cbuf->head) {
        cbuf->cline = cbuf->head->next;
    }
    update_tline(cwin);
    cbuf->head = cbuf->head->next;
    cbuf->head->prev = NULL;
    cbuf->nlines--;
}

static void line_remove_tail() {
    if (cbuf->cline == cbuf->tail) {
        cbuf->cline = cbuf->tail->prev;
    }
    cbuf->tail = cbuf->tail->prev;
    cbuf->tail->next = NULL;
    cbuf->nlines--;
}

static void line_empty(Line *line) {
    free(line->content);
    line->content = NULL;
    line->size = 0;
}

void line_remove(Line *del) {
    if (cbuf->nlines == 1) {
        line_empty(del);
        return;
    }

    if (del == cbuf->head) {
        line_remove_head();
        free(del);
        return;
    }
    if (del == cbuf->tail) {
        line_remove_tail();
        free(del);
        return;
    }
    if (cbuf->cline == del) {
        cbuf->cline = del->next;
    }
    del->prev->next = del->next;
    del->next->prev = del->prev;
    cbuf->nlines--;
    free_line(del);
}

int buffer_progress(Buffer *buf) {
    return (int)((float)(buf->iline + 1) / buf->nlines * 100);
}
