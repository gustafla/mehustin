#ifndef DEMO_H
#define DEMO_H

#define GET(x) demo_sync_get_value(demo, x)

#include <stddef.h>
#include <stdint.h>
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
	int (*scene_init)(int32_t width, int32_t height);
	void (*scene_deinit)(void);
	void (*scene_render)(double time);
#endif
} demo_t;

int demo_init(player_t *player, int width, int height);
void demo_deinit(void);
void demo_render(void);
int demo_reload(void);
double demo_sync_get_value(const char*);

#endif
