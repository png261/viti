// command.c: handle command mode

#include "command.h"

#include "buffer.h"
#include "fileio.h"
#include "message.h"
#include "mode.h"
#include "search.h"
#include "util.h"
#include "window.h"

#include <string.h>

extern Win *cwin;
extern Buffer *cbuf;
extern bool is_highlight;

void command_mode() {
    const char *query = prompt(":%s", NULL);
    if (query == NULL) {
        mode_switch(MODE_NORMAL);
        return;
    }

    char filename[128];

    if (strcmp(query, "q") == 0) {
        viti_quit();
    } else if (strcmp(query, "w") == 0) {
        file_save(cbuf->name, cbuf);
    } else if (strcmp(query, "x") == 0) {
        file_save(cbuf->name, cbuf);
        viti_quit();
    } else if (strcmp(query, "noh") == 0) {
        is_highlight = false;
        win_render_lines(cwin);
    } else if (sscanf(query, "w %s", filename) == 1) {
        cbuf->name = filename;
        file_save(filename, cbuf);
        win_render_status(cwin);
    }

    mode_switch(MODE_NORMAL);
}
