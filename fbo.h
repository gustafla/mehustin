#ifndef FBO_H
#define FBO_H

#include "gl_util.h"

typedef struct {
    GLuint fbo;
    GLuint *textures;
    size_t textures_len;
} fbo_t;

fbo_t *fbo_init(GLsizei width, GLsizei height, const tex_image_2d_t **params);
void fbo_free(fbo_t *fbo);
void fbo_bind(fbo_t *fbo);
void fbo_bind_textures(fbo_t *fbo);

#endif
