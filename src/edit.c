#include "edit.h"

#include "message.h"
#include "window.h"
#include "util.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>


extern Win *curwin;
extern Buffer *curbuf;


void edit_append_char(Line *line, int col, const char c)
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
    if(line->size == 0) {
        return;
    }

    memmove(&line->content[col], &line->content[col + 1], line->size - col);
    line->size--;
}


char *edit_substr(char *str, int start, int len) 
{
    char *substr = xmalloc(len * sizeof(*substr));
    memcpy(substr, &str[start], len);
    return substr;
}


void edit_del_str(Line *line, int start, int len) 
{
    if(line == NULL) {
        return;
    }
    memmove(&line->content[start], &line->content[start + len], line->size - len);
    line->size -= len;
}


void edit_del_line(Line *line) 
{
    line_remove(line);
}


void edit_append_line(Line *line, const char *str, size_t size) 
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
