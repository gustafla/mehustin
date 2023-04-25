#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "gl.h"

typedef struct primitives_t_ {
    GLuint quad_vertices;
} primitives_t;

void primitives_init(primitives_t *primitives);
void primitives_deinit(primitives_t *primitives);

#endif
