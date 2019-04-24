#ifndef SCENE_H
#define SCENE_H

#include <stdint.h>

int scene_init(int32_t width, int32_t height);
void scene_deinit(void);
void scene_render(double time);

#endif

