#ifndef VITI_EDIT_H
#define VITI_EDIT_H

#include "buffer.h"
#include "window.h"

void edit_append_char(Line *line, int col, const char c);
void edit_append_line(Line *line, const char *str);
void edit_del_char(Line *line, int col);

void edit_join_line(Line *line);
void edit_add_line(Line *line, char * str);
void edit_del_line(Line *line);
void edit_break_line(Line *line, int col);

char * edit_del_end(Line *line, int col);
char * edit_del_str(Line *line, int start, int end); 

#endif
