#ifndef DEMO_H
#define DEMO_H

#define GET(x) demo_sync_get_value(demo, x)

#include <stddef.h>
#include "player.h"
#include "sync.h"

typedef struct {
    double time;
    double row;
    player_t *player;
    struct sync_device *rocket;
    int width;
    int height;
} demo_t;

demo_t *demo_init(player_t*, int, int);
void demo_free(demo_t*);
void demo_render(demo_t*);
void demo_reload(demo_t*);
double demo_sync_get_value(const demo_t*, const char*);

#endif
