#ifndef DEMO_PROCS_H
#define DEMO_PROCS_H

#include <stdint.h>

typedef const void *(*gettrack_t)(const char *name);
typedef double (*getval_t)(const void *track);
typedef void *(*scene_init_t)(int32_t width, int32_t height,
                              gettrack_t gettrack, getval_t getval);
typedef void (*scene_deinit_t)(void *data);
typedef void (*scene_render_t)(double time, void *data);

typedef struct {
#ifdef DEMO_RTDL
    void *module;
#endif
    scene_init_t scene_init;
    scene_deinit_t scene_deinit;
    scene_render_t scene_render;
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
#else
#include "src/scene/scene.h"
#define SCENE_INIT scene_init
#define SCENE_DEINIT scene_deinit
#define SCENE_RENDER scene_render
#endif // defined(DEMO_RTDL)
#endif // !defined(DEMO_PROCS_H)
