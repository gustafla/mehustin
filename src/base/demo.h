#ifndef DEMO_H
#define DEMO_H

#define GET(x) demo_sync_get_value(demo, x)

#include <stddef.h>
#include "player.h"
#include "sync.h"

typedef struct demo_t_ {
	double time;
	double row;
	player_t *player;
	struct sync_device *rocket;
	int width;
	int height;

	// scene module variables
#ifndef DEMO_MONOLITHIC
	void *module;
	int (*scene_init)(const struct demo_t_*);
	void (*scene_free)(void);
	void (*scene_render)(void);
#endif
} demo_t;

demo_t *demo_init(player_t*, int, int);
void demo_free(demo_t*);
void demo_render(demo_t*);
int demo_reload(demo_t*);
double demo_sync_get_value(const demo_t*, const char*);

#endif
