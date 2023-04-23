#ifndef RESOURCES_H
#define RESOURCES_H

#include "gl.h"
#include "shader_code.h"
#include <stddef.h>

size_t read_file_to_str(const char *, char **);
GLuint compile_shader(const char *src, const char *type, const char *defines);
GLuint compile_shader_file(const char *filename, const char *defines);
GLuint link_program(size_t count, GLuint *shaders);

#ifdef MONOLITH
#define SHADER(NAME, TYPE, DEFS) compile_shader(NAME##_##TYPE, #TYPE, DEFS)
#else
#define SHADER(NAME, TYPE, DEFS)                                               \
    compile_shader_file("shaders/" #NAME "." #TYPE, DEFS)
#endif
#endif
