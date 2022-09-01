// edit.c: handle edit line

#include "edit.h"

#include "memory.h"
#include "util.h"
#include "window.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

extern Win *cwin;
extern Buffer *cbuf;

void edit_append_char(Line *line, int col, const char c) {
    line->content = xrealloc(line->content, line->size + 2);
    memmove(&line->content[col] + 1, &line->content[col], line->size - col + 1);
    line->content[col] = c;
    line->size++;
}

void edit_del_char(Line *line, int col) {
    if (line->size == 0) {
        return;
    }

    memmove(&line->content[col], &line->content[col + 1], line->size - col);
    line->size--;
}

char *edit_substr(char *str, int start, int len) {
    char *substr = xmalloc(len * sizeof(*substr));
    memcpy(substr, &str[start], len);
    return substr;
}

void edit_del_str(Line *line, int start, int len) {
    memmove(&line->content[start], &line->content[start + len],
            line->size - len);
    line->size -= len;
}

void edit_append_line(Line *line, const char *str, size_t size) {
    line->content = xrealloc(line->content, line->size + size);
    memcpy(&line->content[line->size], str, size);
    line->size += size;
}

void edit_join_line(Line *line) {
    if (line->next->size != 0) {
        edit_append_line(line, line->next->content, line->next->size);
    }
    line_remove(line->next);
}
