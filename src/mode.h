#ifndef VIIT_MODE_H
#define VITI_MODE_H

enum MODE {
    NORMAL,
    INSERT,
    COMMAND,
    SEARCH,
    PROMPT,
};

void mode_switch(const int mode);

#endif
