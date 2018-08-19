#include "fbo.h"
#include <stdio.h>
#include <stdlib.h>

static const GLuint attachments[] = { GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6,
    GL_COLOR_ATTACHMENT7 };

fbo_t *fbo_init(GLsizei width, GLsizei height, const tex_image_2d_t **formats) {
    fbo_t *fbo = calloc(1, sizeof(fbo_t));
    if (!fbo) goto error;

    glGenFramebuffers(1, &fbo->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);

    fbo->textures = gen_textures(width, height, formats, &fbo->textures_len);
    if (!fbo->textures) goto cleanup_fbo;


    for (size_t i = 0; i < fbo->textures_len; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i],
                formats[i]->target, fbo->textures[i], formats[i]->level);
    }

    glGenRenderbuffers(1, &fbo->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER, fbo->rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "FBO not complete\n");
        goto cleanup_rbo;
    }

    glDrawBuffers(fbo->textures_len, attachments);

    return fbo;

cleanup_rbo:
    glDeleteRenderbuffers(1, &fbo->rbo);
    glDeleteTextures(fbo->textures_len, fbo->textures);
cleanup_fbo:
    glDeleteFramebuffers(1, &fbo->fbo);
    free(fbo);
error:
    return NULL;
}
