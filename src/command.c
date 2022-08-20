#include "command.h"

#include "fileio.h"
#include "mess.h"
#include "mode.h"
#include "search.h"
#include "util.h"
#include <string.h>

extern Win *cwin;
extern int is_highlight;

void commandMode() {
    char *query = prompt(":%s", NULL);
    if (query == NULL) {
        mode_switch(NORMAL);
        return;
    }

    char filename[128];

    if (!strcmp(query, "q")) {
        quit();
    } else if (!strcmp(query, "w")) {
        file_save(cwin->buf);
    } else if (!strcmp(query, "noh")) {
        is_highlight = false;
        win_render_rows(cwin);
    } else if (sscanf(query, "w %s", filename) == 1) {
        cwin->buf->file.name = filename;
        file_save(cwin->buf);
    }

    mode_switch(NORMAL);
}
