#include "fbo.h"
#include <stdio.h>
#include <stdlib.h>

static const GLuint ATTACHMENTS[] = {
    GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7
};

fbo_t *fbo_init(GLsizei width, GLsizei height, const tex_image_2d_t **params) {
    fbo_t *fbo = calloc(1, sizeof(fbo_t));
    if (!fbo) goto error;

    glGenFramebuffers(1, &fbo->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);

    // count needed textures
    for (fbo->textures_len=0; params[fbo->textures_len]; fbo->textures_len++);

    // allocate textures
    fbo->textures = calloc(fbo->textures_len, sizeof(GLuint));
    if (!fbo->textures) goto cleanup_fbo;

    size_t color_att = 0;
    for (size_t i = 0; i < fbo->textures_len; i++) {
        // init
        fbo->textures[i] = gen_texture(width, height, *params[i]);

        // figure out attachment
        GLuint att = ATTACHMENTS[color_att];
        switch (params[i]->format) {
            case GL_DEPTH_STENCIL:   att = GL_DEPTH_STENCIL_ATTACHMENT; break;
            case GL_DEPTH_COMPONENT: att = GL_DEPTH_ATTACHMENT; break;
            default: color_att++; break;
        }

        // attach
        glFramebufferTexture2D(GL_FRAMEBUFFER, att, params[i]->target,
                fbo->textures[i], params[i]->level);
    }

    // check for completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "FBO not complete\n");
        goto cleanup_texture;
    }

    // pre-setup multi-color attachment drawing
    glDrawBuffers(color_att, ATTACHMENTS);

    // unbind
    fbo_bind(NULL);

    return fbo;

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
        free(fbo);
    }
}

void fbo_bind(fbo_t *fbo) {
    if (!fbo) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
    }
}

void fbo_bind_textures(fbo_t *fbo) {
    for (size_t i=0; i < fbo->textures_len; i++) {
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D, fbo->textures[i]);
    }
}
