#include "gl_util.h"
#include "read_file.h"
#include <stdio.h>
#include <stdlib.h>

void print_errors(void) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        switch(err) {
            case GL_INVALID_ENUM:
                fprintf(stderr, "GL_INVALID_ENUM\n");
                break;
            case GL_INVALID_VALUE:
                fprintf(stderr, "GL_INVALID_VALUE\n");
                break;
            case GL_INVALID_OPERATION:
                fprintf(stderr, "GL_INVALID_OPERATION\n");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                fprintf(stderr, "GL_INVALID_FRAMEBUFFER_OPERATION\n");
                break;
            case GL_OUT_OF_MEMORY:
                fprintf(stderr, "GL_OUT_OF_MEMORY\n");
                break;
            default: fprintf(stderr, "Unknown OpenGL error\n"); break;
        }
    }
}

GLuint gen_vao(const vertex_attrib_pointer_t **vaparam) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const vertex_attrib_pointer_t *params;
    while ((params = *vaparam++)) {
        glVertexAttribPointer(params->index, params->size, params->type,
                params->normalized, params->stride, params->pointer);
        glEnableVertexAttribArray(params->index);
    }

    glBindVertexArray(0);

    return vao;
}

GLuint compile_shader(GLenum type, char *shader_file_path) {
    char *source;
    if (!read_file_to_str(shader_file_path, &source)) {
        return 0;
    }

    GLuint shader = glCreateShader(type);
    if (!shader) {
        fprintf(stderr, "glCreateShader failed\n");
        free(source);
        return 0;
    }

    glShaderSource(shader, 1, (const GLchar**)&source, NULL);
    glCompileShader(shader);
    free(source);

    GLint success; GLchar info[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info);
        fprintf(stderr, "%s", info);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint link_program(char *vertex_file_path, char *fragment_file_path) {
    GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertex_file_path);
    if (!vertex) {
        return 0;
    }

    GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fragment_file_path);
    if (!fragment) {
        glDeleteShader(vertex);
        return 0;
    }

    GLuint program = glCreateProgram();
    if (!program) {
        fprintf(stderr, "glCreateProgram failed\n");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glDetachShader(program, vertex);
    glDetachShader(program, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint success; GLchar info[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, info);
        fprintf(stderr, "%s", info);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

void ufm_int(GLuint program, const char *name, int value) {
    glUniform1i(glGetUniformLocation(program, name), value);
}

void ufm_float(GLuint program, const char *name, float value) {
    glUniform1f(glGetUniformLocation(program, name), value);
}

void ufm_mat4(GLuint program, const char *name, mat4 matrix) {
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE,
            (GLfloat*)matrix);
}

GLuint gen_texture(GLsizei width, GLsizei height, tex_image_2d_t params) {
    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(params.target, texture);
    glTexImage2D(params.target, params.level, params.internalformat,
            width, height, 0, params.format, params.type, params.data);
    glTexParameteri(params.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(params.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(params.target, 0);

    return texture;
}
