#include "edit.h"
#include <stdlib.h>
#include <string.h>

extern Buffer *cbuf;

void append_char(int line, int col, char c) {
    Row *row = &cbuf->rows[line];
    row->content = realloc(row->content, row->size + 2);
    memmove(&row->content[col + 1], &row->content[col], row->size - col + 1);
    row->content[col] = c;
    row->size++;
    buffer_render_rows(cbuf);
    cursor_right(cbuf);
}

void append_str(int line, char *str) {
    Row *row = &cbuf->rows[line];
    int len = strlen(str);
    row->content = realloc(row->content, row->size + len + 1);
    memcpy(&row->content[row->size], str, len);
    row->size += len;
    row->content[row->size] = '\0';
}

void join_line(int line) {
    if (line == 0) {
        return;
    }
    Row *row = &cbuf->rows[line];
    cbuf->cur.x = (row - 1)->size;
    cbuf->cur.y--;
    append_str(line - 1, row->content);
    del_line(cbuf->view.line);
    buffer_render_rows(cbuf);
    cursor_refresh(cbuf);
}

void del_char(int line, int col) {
    Row *row = &cbuf->rows[line];
    if (col >= row->size) {
        return;
    }

    if (col + 1 == 0) {
        join_line(line);
        return;
    }

    memmove(&row->content[col], &row->content[col + 1], row->size - col);
    row->size--;
    buffer_render_rows(cbuf);
    cursor_left(cbuf);
}

void del_str(int line, int start, int end) {
    Row *row = &cbuf->rows[line];
    int len = end - start;
    memmove(&row->content[start], &row->content[end + 1], row->size - len + 1);
    row->size -= len;
    buffer_render_rows(cbuf);
    cursor_refresh(cbuf);
}

void del_end(int line, int col){
    del_str(line, col, cbuf ->rows[cbuf -> view.line].size);
}

void add_line(int line, char *str) {
    cbuf->rows = realloc(cbuf->rows, (cbuf->file.lines + 1) * sizeof(Row));
    memmove(&cbuf->rows[line + 1], &cbuf->rows[line],
            (cbuf->file.lines - line) * sizeof(Row));
    int len = strlen(str);
    cbuf->rows[line].size = len;
    cbuf->rows[line].content = malloc(sizeof(str));
    memcpy(cbuf->rows[line].content, str, len);
    cbuf->file.lines++;
    buffer_render_rows(cbuf);
    cursor_refresh(cbuf);
}

void del_line(int line) {
    if (line < 0 || line >= cbuf->file.lines - 1) {
        return;
    }
    Row *row = &cbuf->rows[line];
    free(row->content);
    memmove(row, row + 1, sizeof(Row) * (cbuf->file.lines - line - 1));
    cbuf->file.lines--;
}
