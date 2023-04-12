#ifndef SCENE_H
#define SCENE_H

#include <stdint.h>

void *scene_init(int32_t width, int32_t height,
                 const void *(*gettrack)(const char *),
                 double (*getval)(const void *));
void scene_deinit(void *data);
void scene_render(double time, void *data);

#endif
