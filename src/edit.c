#include "edit.h"
#include <stdlib.h>
#include <string.h>

extern Buffer *cbuf;

/* char */
void append_char(int line, int col, char c) {
    Row *row = &cbuf->rows[line];
    if (col < 0 || col > row->size) {
        col = row->size;
    }

    row->content = realloc(row->content, row->size + 2);
    memmove(&row->content[col + 1], &row->content[col], row->size - col + 1);
    row->content[col] = c;
    row->size++;
    buffer_render_rows(cbuf);
    cursor_right(cbuf);
}

void del_char(int line, int col) {
    Row *row = &cbuf->rows[line];
    if (col + 1 == 0) {
        join_line(line);
        return;
    }

    if (col < 0 || col >= row->size) {
        return;
    }

    memmove(&row->content[col], &row->content[col + 1], row->size - col);
    row->size--;
    buffer_render_rows(cbuf);
}

/* line */
char *get_substring(char *str, int start, int len) {
    char *substr = malloc((len) * sizeof(char));
    memcpy(substr, &str[start], len);
    substr[len] = '\0';
    return substr;
}

char *del_str(int line, int start, int end) {
    if (start < 0 || end > cbuf->rows[line].size) {
        return NULL;
    }

    Row *row = &cbuf->rows[line];
    int len = end - start + 1;
    char *deleted_str = get_substring(row->content, start, len);
    memmove(&row->content[start], &row->content[end], row->size - len);

    row->size -= len;
    buffer_render_rows(cbuf);
    cursor_refresh(cbuf);
    return deleted_str;
}

void add_line(int line, char *str) {
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

    buffer_render_rows(cbuf);
    cursor_refresh(cbuf);
}

void del_line(int line) {
    if (line < 0 || line >= cbuf->file.lines) {
        return;
    }
    Row *row = &cbuf->rows[line];
    free(row->content);
    memmove(row, row + 1, sizeof(Row) * (cbuf->file.lines - line - 1));
    cbuf->file.lines--;
}

void append_line(int line, char *str) {
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
    cbuf->cur.y = line - 1;

    append_line(line - 1, row->content);
    del_line(line);

    buffer_render_rows(cbuf);
    cursor_refresh(cbuf);
}

char *del_end(int line, int col) {
    return del_str(line, col, cbuf->rows[line].size - 1);
}

void break_line(int line, int col) {
    char *str = del_end(line, col);
    add_line(line + 1, str);
}
