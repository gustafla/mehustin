#ifndef SCENE_H
#define SCENE_H

#include <stdint.h>

typedef struct tracks_t_ {
    const void *brightness;
    const void *cam_pos_x;
    const void *cam_pos_y;
    const void *cam_pos_z;
    const void *cam_dir_x;
    const void *cam_dir_y;
    const void *cam_dir_z;
    const void *cam_focus;
} tracks_t;

void *scene_init(int32_t width, int32_t height,
                 const void *(*gettrack)(const char *),
                 double (*getval)(const void *));
void scene_deinit(void *data);
void scene_render(void *data, double time);
void scene_resize(void *data, uint32_t width, uint32_t height);

#endif
