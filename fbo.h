#ifndef FBO_H
#define FBO_H

#include "gl_util.h"

#define FBO_TEX(internal_format, format) (tex_image_2d_t)\
{GL_TEXTURE_2D, 0, internal_format, format, GL_UNSIGNED_BYTE, NULL}

typedef struct {
    GLuint fbo;
    GLuint *textures;
    size_t textures_len;
    GLuint *rbos;
    size_t rbos_len;
} fbo_t;

typedef struct {
    GLenum target;
    GLenum attachment;
    GLenum internalformat;
} rbo_binding_format_t;

fbo_t *fbo_init(GLsizei width, GLsizei height,
        const tex_image_2d_t **texture_params,
        const rbo_binding_format_t **rbo_params);
void fbo_free(fbo_t *fbo);

#endif
