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

extern Win *curwin;
extern Buffer *curbuf;
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
        file_save(curbuf->name, curbuf);
    } else if (strcmp(query, "x") == 0) {
        file_save(curbuf->name, curbuf);
        viti_quit();
    } else if (strcmp(query, "noh") == 0) {
        is_highlight = false;
        win_render_lines(curwin);
    } else if (sscanf(query, "w %s", filename) == 1) {
        curbuf->name = filename;
        file_save(filename, curbuf);
        win_render_status(curwin);
    }

    mode_switch(MODE_NORMAL);
}
