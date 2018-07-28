#ifndef DEMO_H
#define DEMO_H

#define GET(x) demo_sync_get_value(demo, x)

#include <stddef.h>
#include "player.h"
#include "sync.h"

typedef struct {
    float time;
    float row;
    player_t *player;
    struct sync_device *rocket;
} demo_t;

demo_t *demo_init();
void demo_free(demo_t*);
void demo_render(demo_t*);
void demo_recompile(demo_t*);
float demo_sync_get_value(const demo_t*, const char*);

#endif
