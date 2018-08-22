#include "fbo.h"
#include <stdio.h>
#include <stdlib.h>

static const GLuint ATTACHMENTS[] = {
    GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7
};

fbo_t *fbo_init(GLsizei width, GLsizei height,
        const tex_image_2d_t **texture_params,
        const rbo_binding_format_t **rbo_params) {
    fbo_t *fbo = calloc(1, sizeof(fbo_t));
    if (!fbo) goto error;

    glGenFramebuffers(1, &fbo->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);

    // count needed textures
    for (fbo->textures_len=0; texture_params[fbo->textures_len];
            fbo->textures_len++);

    // init textures
    fbo->textures = gen_textures(width, height, texture_params,
            fbo->textures_len);
    if (!fbo->textures) goto cleanup_fbo;

    // attach textures
    for (size_t i = 0; i < fbo->textures_len; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, ATTACHMENTS[i],
                texture_params[i]->target, fbo->textures[i],
                texture_params[i]->level);
    }

    // count needed rbos
    for (fbo->rbos_len=0; rbo_params[fbo->rbos_len]; fbo->rbos_len++);

    // allocate rbo handles
    fbo->rbos = calloc(fbo->rbos_len, sizeof(GLuint));
    if (!fbo->rbos) goto cleanup_texture;

    // generate and attach rbos
    glGenRenderbuffers(fbo->rbos_len, fbo->rbos);
    for (size_t i = 0; i < fbo->rbos_len; i++) {
        glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbos[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, rbo_params[i]->internalformat,
                width, height);
        glFramebufferRenderbuffer(rbo_params[i]->target,
                rbo_params[i]->attachment, GL_RENDERBUFFER, fbo->rbos[i]);
    }

    // check for completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "FBO not complete\n");
        goto cleanup_rbo;
    }

    // pre-setup multi- color attachment drawing
    glDrawBuffers(fbo->textures_len, ATTACHMENTS);

    return fbo;

cleanup_rbo:
    glDeleteRenderbuffers(fbo->rbos_len, fbo->rbos);
cleanup_texture:
    glDeleteTextures(fbo->textures_len, fbo->textures);
cleanup_fbo:
    glDeleteFramebuffers(1, &fbo->fbo);
    free(fbo);
error:
    return NULL;
}

void fbo_free(fbo_t *fbo) {
    if (fbo) {
        glDeleteTextures(fbo->textures_len, fbo->textures);
        glDeleteRenderbuffers(fbo->rbos_len, fbo->rbos);
        free(fbo);
    }
}
