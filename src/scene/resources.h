#ifndef RESOURCES_H
#define RESOURCES_H

#include "gl.h"
#include <stddef.h>

#ifdef MONOLITH
#include "shader_code.h"
#endif

size_t read_file_to_str(const char *, char **);
GLuint compile_shader(GLenum, const char *);
GLuint compile_shader_file(GLenum, const char *);

#ifdef MONOLITH
#define SHADER(GLTYPE, NAME, TYPE) compile_shader(GLTYPE, NAME##_##TYPE)
#else
#define SHADER(GLTYPE, NAME, TYPE) compile_shader_file(GLTYPE, #NAME "." #TYPE)
#endif

#endif