#include "edit.h"
#include "mess.h"
#include "window.h"
#include <stdlib.h>
#include <string.h>

extern Win *cwin;

/* char */
void append_char(int line, int col, char c) {
    Row *row = &cwin->buf->rows[line];
    if (col < 0 || col > row->size) {
        col = row->size;
    }

    row->content = realloc(row->content, row->size + 2);
    memmove(&row->content[col + 1], &row->content[col], row->size - col + 1);
    row->content[col] = c;
    row->size++;
    win_render_rows(cwin);
    cursor_right(cwin);
}

void del_char(int line, int col) {
    Row *row = &cwin->buf->rows[line];
    if (col == 0) {
        join_line(line);
        return;
    }

    memmove(&row->content[col], &row->content[col + 1], row->size - col);
    row->size--;
    win_render_rows(cwin);
}

/* line */
char *get_substring(char *str, int start, int len) {
    char *substr = malloc((len) * sizeof(char));
    memcpy(substr, &str[start], len);
    substr[len] = '\0';
    return substr;
}

char *del_str(int line, int start, int end) {
    Row *row = &cwin->buf->rows[line];
    if (start < 0 || end > row->size) {
        return NULL;
    }

    int len = end - start + 1;
    char *deleted_str = get_substring(row->content, start, len);

    if (start < end) {
        memmove(&row->content[start], &row->content[end], row->size - len);
    }

    row->size -= len;
    win_render_rows(cwin);
    cursor_refresh(cwin);
    return deleted_str;
}

void add_line(int line, char *str) {
    Buffer *cbuf = cwin->buf;
    cbuf->rows = realloc(cbuf->rows, (cbuf->file.lines + 1) * sizeof(Row));

    if (line < cbuf->file.lines) {
        memmove(&cbuf->rows[line + 1], &cbuf->rows[line],
                (cbuf->file.lines - line) * sizeof(Row));
    }

    int len = strlen(str);
    Row *row = &cbuf->rows[line];
    row->content = malloc(sizeof(str));
    memcpy(row->content, str, len);
    row->size = len;
    row->content[row->size] = '\0';
    cbuf->file.lines++;

    win_render_rows(cwin);
    cursor_refresh(cwin);
}

void del_line(int line) {
    Buffer *cbuf = cwin->buf;
    if (line < 0 || line >= cbuf->file.lines) {
        return;
    }
    Row *row = &cbuf->rows[line];
    free(row->content);
    memmove(row, row + 1, sizeof(Row) * (cbuf->file.lines - line - 1));
    cbuf->file.lines--;
}

void append_line(int line, char *str) {
    Row *row = &cwin->buf->rows[line];
    int len = strlen(str);
    row->content = realloc(row->content, row->size + len + 1);
    memcpy(&row->content[row->size], str, len);
    row->size += len;
    row->content[row->size] = '\0';
}

void join_line(int line) {
    Row *row = &cwin->buf->rows[line];
    if (line == 0) {
        return;
    }

    cwin->cur.x = (row - 1)->size;
    cwin->cur.y = line - 1;

    append_line(line - 1, row->content);
    del_line(line);

    win_render_rows(cwin);
    cursor_refresh(cwin);
}

char *del_end(int line, int col) {
    Row *row = &cwin->buf->rows[line];
    return del_str(line, col, row->size - 1);
}

void break_line(int line, int col) {
    char *str = del_end(line, col);
    add_line(line + 1, str);
}
