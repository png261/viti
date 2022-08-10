#include "command.h"
#include "file_io.h"
#include "mess.h"
#include "util.h"
#include "buffer.h"
#include "mode.h"

extern Buffer * cbuf;

void commandMode() {
    char *query = prompt(":%s");
    switch (*query) {
    case 'q':
        quit();
        break;
    case 'w':
        file_save(cbuf);
        mess_send("saved");
        break;
    }
    mode_switch(NORMAL);
}


