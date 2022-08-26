#include "edit.h"

#include "message.h"
#include "window.h"
#include "util.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>

extern Win *curwin;
extern Buffer *curbuf;

void edit_append_char(Line * line, int col, const char c)
{
    if(line == NULL) {
        return;
    }

    line->content = xrealloc(line->content, line->size + 2);
    memmove(&line->content[col] + 1, &line->content[col], line->size - col + 1);
    line->content[col] = c;
    line->size++;
}


void edit_del_char(Line *line, int col) 
{
    if(line == NULL || col == line->size) {
        return;
    }

    memmove(&line->content[col], &line->content[col] + 1, line->size - col);
    line->size--;
}

char *edit_get_substring(const char *str, int start, int len) 
{
    char *substr = xmalloc((len) * sizeof(*substr));
    memcpy(substr, &str[start], len);
    return substr;
}


char *edit_del_str(Line * line, int start, int len) 
{
    char *deleted_str = edit_get_substring(line->content, start, len);
    memmove(&line->content[start], &line->content[start] + len, line->size - len);
    line->size -= len;
    return deleted_str;
}


void edit_del_line(Line *line) 
{
    line_remove(&curbuf->lines, line);
    curbuf->nlines--;
}


void edit_append_line(Line * line, const char *str, size_t size) 
{
    if(line == NULL) {
        return;
    }

    line->content = xrealloc(line->content, line->size + size);
    memcpy(&line->content[line->size], str, size);
    line->size += size;
}


void edit_join_line(Line *line) 
{
    if(line == NULL) {
        return;
    }

    if(line->next->size != 0) {
        edit_append_line(line, line->next->content, line->next->size);
    }
    edit_del_line(line->next);
}
