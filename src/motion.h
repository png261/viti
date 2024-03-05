#ifndef VITI_MOTION_H
#define VITI_MOTION_H

void move_end_line();
void move_start_line();
void move_screen(int lines);
void move_cursor(int lines);
void go_top();
void go_bottom();
int nextrune(int inc);
void move_word_forward();
void move_end_word_forward();
void move_word_backward();
void find_char(int inc, char c);

#endif // VITI_MOTION_H
