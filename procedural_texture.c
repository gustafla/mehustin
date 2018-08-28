#include "procedural_texture.h"
#include "cglm/cglm.h"
#include "fbo.h"
#include "gl_util.h"
#include "quad.h"

static mat4 identity = GLM_MAT4_IDENTITY_INIT;

GLuint texture_render(GLsizei w, GLsizei h, char *fragment_path,
        tex_image_2d_t format) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, w, h);

    const tex_image_2d_t *formats[] = {&format, NULL};
    fbo_t *fbo = fbo_init(w, h, formats);
    if (!fbo) return EXIT_FAILURE;

    GLuint program = link_program("vertex.vert", fragment_path);
    if (!program) {
        fbo_free(fbo);
        return EXIT_FAILURE;
    }
    ufm_mat4(program, "model", identity);
    ufm_mat4(program, "view", identity);
    ufm_mat4(program, "projection", identity);

    fbo_bind(fbo);
    glUseProgram(program);
    quad_render();
    glDeleteProgram(program);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    return fbo_free_keeptexture(fbo);
}

