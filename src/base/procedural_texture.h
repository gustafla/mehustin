#ifndef PROCEDURAL_TEXTURE_H
#define PROCEDURAL_TEXTURE_H

#include "fbo.h"
#include "gl_util.h"

GLuint texture_render(GLsizei w, GLsizei h, char *fragment_path,
		tex_image_2d_t format);

#endif
