// motion.c: handle motion

#include "motion.h"

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "fileio.h"
#include "key.h"
#include "message.h"
#include "mode.h"
#include "search.h"
#include "util.h"
#include "window.h"

#include <ctype.h>
#include <string.h>

extern Win *cwin;
extern Buffer *cbuf;

void move_end_line() {
    cbuf->icol = cbuf->cline->size;
    cursor_refresh(cwin);
}

void move_start_line() {
    cbuf->icol = 0;
    cursor_refresh(cwin);
}

void move_screen(int lines) {
    cwin->lineoff = MAX(0, cwin->lineoff + lines);
    cbuf->iline += lines;
    win_scroll(cwin);
}

void move_cursor(int lines) {
    cbuf->iline = cwin->lineoff + lines;
    cursor_refresh(cwin);
}

void go_top() {

    cbuf->iline = 0;
    cbuf->cline = cbuf->head;
    win_scroll(cwin);
    win_render_num(cwin);
    win_render_status(cwin);
    cursor_refresh(cwin);
}

void go_bottom() {
    cbuf->iline = cbuf->nlines - 1;
    cbuf->cline = cbuf->tail;
    win_scroll(cwin);
    win_render_num(cwin);
    win_render_status(cwin);
    cursor_refresh(cwin);
}

int nextrune(int inc) {
    int n;
    for (n = cbuf->icol + inc;
         (n + inc >= 0) && (cbuf->cline->content[n] & 0xc0) == 0x80; n += inc)
        ;
    return n;
}

void move_word_forward() {
    const char *text = cbuf->cline->content;

    if ((cbuf->icol == cbuf->cline->size) && (cbuf->iline == cbuf-> nlines - 1)) {
        return;
    }

    if (cbuf->icol >= cbuf->cline->size) {
        cbuf->icol = 0;
        cbuf->iline++;
        cbuf->cline = cbuf->cline->next;
    } else {
        while (strchr(" ", text[cbuf->icol])) {
            cbuf->icol++;
        }
        while (!strchr(" ", text[cbuf->icol])) {
            cbuf->icol++;
        }
        cbuf->icol++;
    }

    win_render_lines(cwin);
}

void move_end_word_forward() {
    const char *text = cbuf->cline->content;

    if ((cbuf->icol == cbuf->cline->size) && (cbuf->iline == cbuf-> nlines - 1)) {
        return;
    }

    cbuf->icol++;
    if (cbuf->icol > cbuf->cline->size) {
        cbuf->icol = 0;
        cbuf->iline++;
        cbuf->cline = cbuf->cline->next;
    } else {
        while (strchr(" ", text[cbuf->icol])) {
            cbuf->icol++;
        }
        while (!strchr(" ", text[cbuf->icol])) {
            cbuf->icol++;
        }
        cbuf->icol--;
    }

    win_render_lines(cwin);
}

void move_word_backward() {
    const char *text = cbuf->cline->content;

    if ((cbuf->icol == 0) && (cbuf->iline ==  0)) {
        return;
    }

    if ((cbuf->icol <= 0) && (cbuf->iline > 0)) {
        cbuf->iline--;
        cbuf->cline = cbuf->cline->prev;
        cbuf->icol = cbuf->cline->size;
    } else {
        while (strchr(" ", text[nextrune(-1)])) {
            cbuf->icol--;
        }
        while (!strchr(" ", text[nextrune(-1)])) {
            cbuf->icol--;
        }
    }
    win_render_lines(cwin);
}

void find_char(int inc, char c) {
    int n = cbuf->icol + inc;
    while ((n > 0) && (n < cbuf->cline->size)) {
        if (cbuf->cline->content[n] == c) {
            cbuf->icol = n;
            win_render_lines(cwin);
            return;
        }
        n += inc;
    }
}
