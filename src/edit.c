#include "edit.h"

#include "message.h"
#include "window.h"
#include "util.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>

extern Win *curwin;

/* char */
void edit_append_char(int line, int col, const char c)
{
    Row *row = &curwin->buf->rows[line];
    if(row == NULL) {
        return;
    }

    if (col < 0 || col > row->size) {
        col = row->size;
    }

    row->content = xrealloc(row->content, row->size + 2);
    memmove(&row->content[col + 1], &row->content[col], row->size - col + 1);
    row->content[col] = c;
    row->size++;
    curwin->buf->col++;
    win_render_line(curwin, line);
}

void edit_del_char(int line, int col) {
    Row *row = &curwin->buf->rows[line];
    if(row == NULL) {
        return;
    }

    if (col < 0) {
        edit_join_line(line);
        return;
    }

    memmove(&row->content[col], &row->content[col + 1], row->size - col);
    row->size--;
    curwin->buf->col--;
    win_render_line(curwin, line);
}

/* line */
char *edit_get_substring(const char *str, int start, int len) {
    char *substr = xmalloc((len) * sizeof(*substr));
    memcpy(substr, &str[start], len);
    substr[len] = '\0';
    return substr;
}

char *edit_del_str(int line, int start, int end) {
    Row *row = &curwin->buf->rows[line];
    if(row == NULL){
        return NULL;
    }

    if (start < 0 || end > row->size) {
        return NULL;
    }

    int len = end - start + 1;
    char *deleted_str = edit_get_substring(row->content, start, len);

    if (start < end) {
        memmove(&row->content[start], &row->content[end], row->size - len);
    }

    row->size -= len;
    win_render_line(curwin, line);

    return deleted_str;
}

void edit_add_line(int line, const char *str) {
    Buffer *buf = curwin->buf;
    buf->rows = xrealloc(buf->rows, (buf->file.lines + 1) * sizeof(*buf->rows));

    if (line < buf->file.lines) {
        memmove(&buf->rows[line + 1], &buf->rows[line],
                (buf->file.lines - line) * sizeof(*buf->rows));
    }

    const int len = strlen(str);
    Row *row = &buf->rows[line];
    if(row == NULL) {
        return;
    }
    row->content = xmalloc(sizeof(str));
    memcpy(row->content, str, len);
    row->size = len;
    row->content[row->size] = '\0';
    buf->file.lines++;
    win_render_rows(curwin);
}

void edit_del_line(int line) {
    Buffer *buf = curwin->buf;
    Row *row = &buf->rows[line];
    if(row == NULL) {
        return;
    }

    if (line < 0 || line >= buf->file.lines) {
        return;
    }

    free(row->content);
    row->content = NULL;

    memmove(row, row + 1, sizeof(Row) * (buf->file.lines - line - 1));
    buf->file.lines--;
    win_render_rows(curwin);
}

void edit_append_line(int line, const char *str) {
    Row *row = &curwin->buf->rows[line];
    if(row == NULL) {
        return;
    }

    const int len = strlen(str);
    row->content = xrealloc(row->content, row->size + len + 1);
    memcpy(&row->content[row->size], str, len);
    row->size += len;
    row->content[row->size] = '\0';
    win_render_line(curwin, line);
}

void edit_join_line(int line) {
    Row *row = &curwin->buf->rows[line];
    if(row == NULL) {
        return;
    }

    if(line == 0 && row->size <= 0) {
        edit_del_line(line);
        return;
    }

    if (line == 0) {
        return;
    }


    curwin->buf->col = (row - 1)->size + 1;
    curwin->buf->line = line - 1;

    edit_append_line(line - 1, row->content);
    edit_del_line(line);
}

char *edit_del_end(int line, int col) {
    Row *row = &curwin->buf->rows[line];
    if(row == NULL) {
        return NULL;
    }
    return edit_del_str(line, col, row->size - 1);
}

void edit_break_line(int line, int col) {
    const char *str = edit_del_end(line, col);
    edit_add_line(line + 1, str);
}
