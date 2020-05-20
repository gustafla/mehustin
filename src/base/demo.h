#ifndef DEMO_H
#define DEMO_H

#define GET(x) demo_sync_get_value(x)

#include "player.h"

int demo_init(player_t *player, int width, int height, double bpm, double rpb);
void demo_deinit(void);
void demo_render(void);
int demo_reload(void);
const void *demo_sync_get_track(const char*);
double demo_sync_get_value(const void*);

#endif
