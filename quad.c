#include "quad.h"
#include "gl_util.h"

static int initialized = 0;

static GLuint vao;
static GLuint buffer;

// the quad geometry
static GLfloat quad[] = {
    -1, -1, 0, 0, 0,
    -1,  1, 0, 0, 1,
     1, -1, 0, 1, 0,
     1,  1, 0, 1, 1
};

void quad_render(GLuint program) {
    if (!initialized) {
        // buffer
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
        // vao
        const vertex_attrib_pointer_t *params[] = {
            &VAP(0, 3, 5 * sizeof(GLfloat), 0),
            &VAP(1, 2, 5 * sizeof(GLfloat), 3*sizeof(GLfloat)),
            NULL
        };
        vao = gen_vao(params);
        initialized = 1;
    }

    glUseProgram(program);
    glUniform1i(UNIFORM("texture_0"), 0);
    glUniform1i(UNIFORM("texture_1"), 1);
    glUniform1i(UNIFORM("texture_2"), 2);
    glUniform1i(UNIFORM("texture_3"), 3);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
