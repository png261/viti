#include "buffer.h"

#include "memory.h"
#include "message.h"

#include <stdlib.h>
#include <string.h>


Buffer * curbuf;


Buffer *buffer_create() 
{
    Buffer *buf = xmalloc(sizeof(*buf));
    buf->head = NULL;
    buf->tail = NULL;
    buf->curline = NULL;

    buf->iline = 0;
    buf->icol = 0;

    buf->nlines = 0;
    buf->name = NULL;
    return buf;
}


static Line * new_line(char *content, size_t size)
{
    Line * line = malloc(sizeof(*line));
    line->content = malloc(size * sizeof(*line->content));
    memcpy(line->content, content, size);
    line->size = size;
    line->prev = NULL;
    line->next = NULL;
    return line;
}


void line_push(Line **head, Line **tail, char *content, size_t size)
{
    Line *new = new_line(content, size);
    if(*head == NULL) {
        *head = new;
    } else {
        (*tail)->next = new;
        new->prev = *tail;
    }
    *tail  = new;
}


Line *line_at(Line *line, int at)
{
    while(at > 0) {
        if(line == NULL){
            return NULL;
        }
        line = line->next;
        at--;
    }
    return line;
}


void line_insert_after(Line *root, Line **tail, char *content, size_t size)
{
    if(root == NULL) {
        return;
    }

    Line *new = new_line(content, size);
    new->next = root->next;
    root->next = new;
    new->prev = root;
    if(root == *tail){
        *tail = new;
    } else {
        new->next->prev = new;
    }
}


void line_insert_before(Line ** head, Line * root, char * content, size_t size)
{
    if(root == NULL) {
        return;
    }
    Line *new = new_line(content, size);
    new->next = root;
    if(root == *head) {
        *head = new;
    } else {
        new->prev = root->prev;
        new->prev->next = new;
    } 
    root->prev = new;
}

static void free_line(Line *line)
{
    line->size = 0;
    free(line->content);
    free(line);
}


void line_remove(Line ** head, Line **tail, Line *del)
{
    if(*head == NULL || del == NULL) {
        return;
    }
    
    if(del == *head) {
        *head = del->next;
    } else {
        del->prev->next = del->next;
    }

    if(del == *tail) {
        *tail = del->prev; 
    } else {
        del->next->prev = del->prev;
    }

    free_line(del);
}


int buffer_progress(Buffer *buf)
{
    return (int)((float)(buf->iline + 1) / buf->nlines * 100);
}
