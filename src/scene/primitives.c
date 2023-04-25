#include "primitives.h"
#include <stddef.h>

static size_t n_primitives(void) {
    return sizeof(primitives_t) / sizeof(GLuint);
}

void primitives_init(primitives_t *primitives) {
    glGenBuffers(n_primitives(), (GLuint *)primitives);

    static const GLfloat quad[] = {-1.f, -1., 0., 0., 0., 1.,  -1., 0., 1., 0.,
                                   1.,   1.,  0., 1., 1., -1., -1., 0., 0., 0.,
                                   1.,   1.,  0., 1., 1., -1., 1.,  0., 0., 1.};
    glBindBuffer(GL_ARRAY_BUFFER, primitives->quad_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), (const GLvoid *)quad,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void primitives_deinit(primitives_t *primitives) {
    if (primitives) {
        glDeleteBuffers(n_primitives(), (GLuint *)primitives);
    }
}
