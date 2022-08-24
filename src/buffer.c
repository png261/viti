#include "buffer.h"

#include "memory.h"
#include "message.h"

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

Line *line_insert_after(Line **line, char *content, size_t size)
{
    Line * new = new_line(line, content, size);
    if (*line == NULL) {
        *line = new;
        return;
    }

    Line * temp = *line;
    new->next = temp->next;
    temp->next = new;
    new->prev = temp;
}
Line *line_push(Line **line, char *content, size_t size)
{
    Line * new = new_line(line, content, size);
    if (*line == NULL) {
        *line = new;
        return;
    }
    Line * temp = *line;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new;
    new->prev = temp;
}

Line *line_at(Line *line, int at)
{
    if(line == NULL){
        return NULL;
    }
    return at == 0 ? line : line_at(line->next, at - 1);
}

void line_insert(Line *prev, char *content, size_t size)
{
    if(prev == NULL) {
        return;
    }

    Line *new = new_line(prev, content, size);
    new->next = prev->next;
    prev->next = new;
    if(new->next != NULL){
        new->next->prev = new;
    }
}

void free_line(Line *line)
{
    line->size = 0;
    free(line->content);
    free(line);
}

void line_remove(Line ** head, Line *del)
{
    if(*head == NULL || del == NULL) {
        return;
    }
    
    if(*head == del) {
        *head = del->next;
    }

    if(del->next != NULL) {
        del->next->prev = del->prev;
    } 

    if(del->prev != NULL) {
        del->prev->next = del->next;
    } 

    free_line(del);
}
