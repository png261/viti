#include "edit.h"
#include <stdlib.h>
#include <string.h>

void append_char(Buffer *buf, int line, int col, char c) {
    Row *row = &buf->rows[line];
    row->content = realloc(row->content, row->size + 2);
    memmove(&row->content[col + 1], &row->content[col], row->size - col + 1);
    row->content[col] = c;
    row->size++;
    buffer_render_rows(buf);
    cursor_right(buf);
}

void append_str(Buffer *buf, int line, char *str) {
    Row *row = &buf->rows[line];
    int len = strlen(str);
    row->content = realloc(row->content, row->size + len + 1);
    memcpy(&row->content[row->size], str, len);
    row->size += len;
    row->content[row->size] = '\0';
}

void del_char(Buffer *buf, int line, int col) {
    Row *row = &buf->rows[line];
    if (col < 0 || col > row->size) {
        buf->cur.x = (row - 1)->size;
        buf->cur.y--;
        append_str(buf, line - 1, row->content);
        del_line(buf, buf->view.line);
        buffer_render_rows(buf);
        cursor_refresh(buf);
        return;
    }
    memmove(&row->content[col], &row->content[col + 1], row->size - col);
    row->size--;
    buffer_render_rows(buf);
    cursor_left(buf);
}

void add_line(Buffer *buf, int line) {
    buf->rows = realloc(buf->rows, (buf->file.lines + 1) * sizeof(Row));
    memmove(&buf->rows[line + 1], &buf->rows[line],
            (buf->file.lines - line) * sizeof(Row));
    buf->rows[line].size = 0;
    buf->rows[line].content = malloc(sizeof(char));
    buf->file.lines++;
    buffer_render_rows(buf);
    cursor_refresh(buf);
}

void del_line(Buffer *buf, int line) {
    if (line < 0 || line >= buf->file.lines - 1) {
        return;
    }
    Row *row = &buf->rows[line];
    free(row->content);
    memmove(row, row + 1, sizeof(Row) * (buf->file.lines - line - 1));
    buf->file.lines--;
}
