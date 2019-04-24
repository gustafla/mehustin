#ifndef DEMO_H
#define DEMO_H

#define GET(x) demo_sync_get_value(x)

#include "player.h"

int demo_init(player_t *player, int width, int height);
void demo_deinit(void);
void demo_render(void);
int demo_reload(void);
double demo_sync_get_value(const char*);

#endif
