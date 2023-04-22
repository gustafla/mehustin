#ifndef POST_H
#define POST_H

#include "api.h"
#include "scene.h"
#include "screen_pass.h"

#define NOISE_SIZE 256
#define N_FBOS 3

typedef struct viewport_t_ {
    GLsizei x;
    GLsizei y;
    GLsizei width;
    GLsizei height;
} viewport_t;

typedef struct post_t_ {
    viewport_t output_viewport;
    viewport_t source_viewport;
    float source_aspect_ratio;
    pass_fbo_t fbos[N_FBOS]; // 0: main image, 1: bloom x, 2: bloom y
    pass_t bloomx;
    pass_t bloomy;
    pass_t pass;
    GLuint buffer;
    GLuint vao;
    GLuint noise[2];
    void *noise_buffer;
} post_t;

void post_init(post_t *post, GLsizei width, GLsizei height,
               GLuint vertex_shader);
void post_deinit(post_t *post);
void post_draw(post_t *post, const tracks_t *tr, getval_t get_value);
void post_resize(post_t *post, uint32_t width, uint32_t height);
void post_bind_fbo(const post_t *post);

#endif
