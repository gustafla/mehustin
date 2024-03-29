#ifndef DEMO_PROCS_H
#define DEMO_PROCS_H

#include "api.h"
#include <stdint.h>

typedef struct {
#ifdef DEMO_RTDL
    void *module;
#endif
    scene_init_t scene_init;
    scene_deinit_t scene_deinit;
    scene_render_t scene_render;
    scene_resize_t scene_resize;
} procs_t;

extern procs_t procs;

void procs_deinit(void);
int procs_reload(void);

#ifdef DEMO_RTDL
#define SCENE_INIT procs.scene_init
#define SCENE_DEINIT(data)                                                     \
    if (procs.scene_deinit && data) {                                          \
        procs.scene_deinit(data);                                              \
    }
#define SCENE_RENDER procs.scene_render
#define SCENE_RESIZE procs.scene_resize
#else
#include "scene/scene.h"
#define SCENE_INIT scene_init
#define SCENE_DEINIT(data)                                                     \
    if (data) {                                                                \
        scene_deinit(data);                                                    \
    }
#define SCENE_RENDER scene_render
#define SCENE_RESIZE scene_resize
#endif // defined(DEMO_RTDL)
#endif // !defined(DEMO_PROCS_H)
