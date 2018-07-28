#ifndef GL_UTIL_H
#define GL_UTIL_H

#include <GL/gl.h>
#include <GL/glext.h>

GLuint compile_shader(GLenum, char*);
GLuint link_program(char*, char*);

#endif