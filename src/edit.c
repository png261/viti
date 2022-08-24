#include "edit.h"

#include "message.h"
#include "window.h"
#include "util.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>

extern Win *curwin;
extern Buffer *curbuf;

/* char */
void edit_append_char(Line * line, int col, const char c)
{
    if(line == NULL) {
        return;
    }

    if (col < 0 || col > line->size) {
        col = line->size;
    }

    line->content = xrealloc(line->content, line->size + 2);
    memmove(&line->content[col + 1], &line->content[col], line->size - col + 1);
    line->content[col] = c;
    line->size++;
    line->content[line->size] = '\0';
    curbuf->curcol++;
    win_render_lines(curwin);
}

void edit_del_char(Line *line, int col) {
    if(line == NULL) {
        return;
    }

    if (col < 0) {
        edit_join_line(line);
        return;
    }

    memmove(&line->content[col], &line->content[col + 1], line->size - col);
    line->size--;
    line->content[line->size] = '\0';
    curbuf->curcol--;
    win_render_lines(curwin);
}

/* line */
char *edit_get_substring(const char *str, int start, int len) {
    char *substr = xmalloc((len) * sizeof(*substr));
    memcpy(substr, &str[start], len);
    substr[len] = '\0';
    return substr;
}

char *edit_del_str(Line * line, int start, int end) {
    if(line == NULL) {
        return NULL;
    }

    if (start < 0 || end > line->size) {
        return NULL;
    }

    int len = end - start + 1;
    char *deleted_str = edit_get_substring(line->content, start, len);

    if (start < end) {
        memmove(&line->content[start], &line->content[end], line->size - len);
    }

    line->size -= len;
    win_render_lines(curwin);

    return deleted_str;
}

void edit_add_line(Line *line, char *str) {
    line_insert(line, str, strlen(str));
}

void edit_del_line(Line *line) {
    line_remove(curbuf->lines, line);
}

void edit_append_line(Line * line, const char *str) {
    if(line == NULL) {
        return;
    }

    const int len = strlen(str);
    line->content = xrealloc(line->content, line->size + len + 1);
    memcpy(&line->content[line->size], str, len);
    line->size += len;
    line->content[line->size] = '\0';
    win_render_lines(curwin);
}

void edit_join_line(Line *line) {
    if(line == NULL) {
        return;
    }

    if(line == 0 && line->size <= 0) {
        edit_del_line(line);
        return;
    }

    /* curbuf->curcol = line->size + 1; */
    /* curbuf->curline = line - curbuf->lines - 1; */

    edit_append_line(line->prev, line->content);
    edit_del_line(line);
}

char *edit_del_end(Line *line, int col) {
    if(line == NULL) {
        return NULL;
    }
    return edit_del_str(line, col, line->size - 1);
}

void edit_break_line(Line *line, int col) {
    if(line == NULL){
        return;
    }
    const char *str = edit_del_end(line, col);
    edit_add_line(line + 1, str);
}
