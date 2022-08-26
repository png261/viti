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
    buf->current_line = NULL;
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


void line_push(Line** head_ref, char *content, size_t size)
{
    static Line *current = NULL;
    Line *new_node = new_line(content, size);
    if(*head_ref == NULL) {
        *head_ref = new_node;
    } else {
        current->next = new_node;
        new_node->prev = current;
    }
    current = new_node;
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


void line_insert_after(Line *root, char *content, size_t size)
{
    if(root == NULL) {
        return;
    }

    Line *new = new_line(content, size);
    new->next = root->next;
    root->next = new;
    new->prev = root;
    if(new->next != NULL){
        new->next->prev = new;
    }
}


void line_insert_before(Line ** head_ref, Line * root, char * content, size_t size)
{
    if(root == NULL) {
        return;
    }
    Line *new = new_line(content, size);
    new->next = root;
    if(root->prev != NULL) {
        new->prev = root->prev;
        new->prev->next = new;
    } else {
        *head_ref = new;
    } 
    root->prev = new;
}


void line_add_first(Line ** head, char *content, size_t size)
{
    Line * new = new_line(content, size);
    new->next = (*head);
    (*head)->next->prev = new;
    (*head) = new;
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


void update_current_line(Buffer *buf)
{
    buf->current_line = line_at(buf->lines, buf->curline);
}

