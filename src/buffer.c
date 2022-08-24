#include "buffer.h"

#include "memory.h"

#include <stdlib.h>
#include <string.h>

Buffer * curbuf;

Buffer *buffer_create() 
{
    Buffer *buf = xmalloc(sizeof(*buf));
    buf->curline = 0;
    buf->curcol = 0;
    buf->nlines = 1;
    buf->file.name = NULL;
    buf->lines = NULL;
    return buf;
}

Line * new_line(Line *prev, char *content, size_t size)
{
    Line * line = malloc(sizeof(*line));
    line->content = malloc(size * sizeof(*line->content));
    memcpy(line->content, content, size);
    line->size = size;
    line->prev = prev;
    line->next = NULL;
    return line;
}

Line *line_push(Line **line, char *content, size_t size)
{
    if(*line == NULL) { 
        *line = new_line(*line, content, size);
        return *line;
    } 

    return line_push(&(*line)->next, content, size);
}

