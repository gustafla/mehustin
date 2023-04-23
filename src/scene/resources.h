#ifndef RESOURCES_H
#define RESOURCES_H

#include "gl.h"
#include "shader_code.h"
#include <stddef.h>

size_t read_file_to_str(const char *, char **);
GLuint compile_shader(GLenum, const char *, const char *defines);
GLuint compile_shader_file(GLenum, const char *, const char *defines);
GLuint link_program(size_t count, GLuint *shaders);

#ifdef MONOLITH
#define SHADER(GLTYPE, NAME, TYPE, DEFS)                                       \
    compile_shader(GLTYPE, NAME##_##TYPE, DEFS)
#else
#define SHADER(GLTYPE, NAME, TYPE, DEFS)                                       \
    compile_shader_file(GLTYPE, "shaders/" #NAME "." #TYPE, DEFS)
#endif
#endif
