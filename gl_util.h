#ifndef GL_UTIL_H
#define GL_UTIL_H

#include <GL/gl.h>
#include <GL/glext.h>

#define UNIFORM(x) glGetUniformLocation(program, x)
#define VAP(index, size, stride, pointer) (vertex_attrib_pointer_t)\
{index, size, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)(pointer)}

typedef struct {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    const GLvoid *pointer;
} vertex_attrib_pointer_t;

// generate a vao
GLuint gen_vao(const vertex_attrib_pointer_t**);

// generate a shader object, load from file and compile it
//
// - shader type
// - shader filename
GLuint compile_shader(GLenum, char*);

// load a vertex shader and a fragment shader from a file, compile and link
//
// compiled shader objects will be freed
//
// - vertex shader path
// - fragment shader path
GLuint link_program(char*, char*);

#endif
