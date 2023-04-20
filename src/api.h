#ifndef API_H
#define API_H

#include <stdint.h>

typedef const void *(*gettrack_t)(const char *name);
typedef double (*getval_t)(const void *track);
typedef void *(*scene_init_t)(int32_t width, int32_t height,
                              gettrack_t gettrack, getval_t getval);
typedef void (*scene_deinit_t)(void *data);
typedef void (*scene_render_t)(void *data, double time);
typedef void (*scene_resize_t)(void *data, uint32_t width, uint32_t height);

#endif
