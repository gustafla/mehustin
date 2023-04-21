#ifndef SCREEN_PASS_H
#define SCREEN_PASS_H

#include "gl.h"

typedef struct pass_fbo_t_ {
    GLuint fbo;
    GLuint texture;
} pass_fbo_t;

int pass_fbo_init(pass_fbo_t *fbo, GLsizei width, GLsizei height);
void pass_fbo_deinit(const pass_fbo_t *fbo);

typedef struct pass_t_ {
    GLuint program;
} pass_t;

void pass_bind_program(const pass_t *pass);
int pass_init(pass_t *pass, GLuint vertex_shader, GLuint fragment_shader);
void pass_draw(GLuint vao);
void pass_deinit(const pass_t *pass);

#endif
