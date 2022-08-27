#ifndef VITI_MODE_H
#define VITI_MODE_H

enum MODE {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_COMMAND,
    MODE_SEARCH,
    MODE_PROMPT,
};


void mode_switch(int mode);

#endif // VITI_MODE_H
