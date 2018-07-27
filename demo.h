#ifndef DEMO_H
#define DEMO_H

#include <stddef.h>
#include "player.h"

typedef struct {
    size_t frame;
    float time;
    player_t *player;
} demo_t;

demo_t *demo_init();
void demo_free(demo_t*);
void demo_render(demo_t*);
void demo_recompile(demo_t*);

#endif
