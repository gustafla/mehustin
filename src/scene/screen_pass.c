#include "screen_pass.h"
#include "resources.h"
#include <stdio.h>

int pass_fbo_init(pass_fbo_t *fbo, GLsizei width, GLsizei height) {
    glGenFramebuffers(1, &fbo->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
    glGenTextures(1, &fbo->texture);
    glBindTexture(GL_TEXTURE_2D, fbo->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
                 GL_HALF_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           fbo->texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "FBO not complete\n");
        return -1;
    }

    return 0;
}

void pass_fbo_bind(const pass_fbo_t *fbo) {
    if (fbo == NULL) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

void pass_fbo_bind_tex(const pass_fbo_t *fbo, GLuint n) {
    glActiveTexture(GL_TEXTURE0 + n);
    glBindTexture(GL_TEXTURE_2D, fbo->texture);
}

void pass_fbo_deinit(const pass_fbo_t *fbo) {
    if (fbo) {
        glDeleteFramebuffers(1, &fbo->fbo);
        glDeleteTextures(1, &fbo->texture);
    }
}

int pass_init(pass_t *pass, GLuint vertex_shader, GLuint fragment_shader) {
    pass->program = link_program(2, (GLuint[]){vertex_shader, fragment_shader});
    if (!pass->program) {
        return -1;
    }
    return 0;
}

GLint pass_ufmloc(const pass_t *pass, const char *name) {
    return glGetUniformLocation(pass->program, name);
}

void pass_bind(const pass_t *pass) { glUseProgram(pass->program); }

void pass_draw(GLuint vao) {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void pass_deinit(const pass_t *pass) { glDeleteProgram(pass->program); }
