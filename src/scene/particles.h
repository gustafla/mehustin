#ifndef PARTICLES_H
#define PARTICLES_H

#include "api.h"
#include "gl.h"
#include "primitives.h"
#include "scene.h"
#include <cglm/mat4.h>

#define POINTS 204096

typedef struct particles_t_ {
    GLuint point_instance_buffer;
    GLuint program;
    GLuint vao;

} particles_t;

void particles_init(particles_t *particles, primitives_t *const primitives);
void particles_draw(particles_t *particles, gettrack_t get_track,
                    getval_t get_value, mat4 view, mat4 projection);

#endif
