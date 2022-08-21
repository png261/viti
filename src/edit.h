#ifndef VITI_EDIT_H
#define VITI_EDIT_H

#include "buffer.h"
#include "window.h"

void edit_append_char(int line, int col, const char c);
void edit_append_str(int line, const char *str);
void edit_del_char(int line, int col);

void edit_join_line(int line);
void edit_add_line(int line, const char * str);
void edit_del_line(int line);
void edit_break_line(int line, int col);

char * edit_del_end(int line, int col);
char * edit_del_str(int line, int start, int end); 

#endif
