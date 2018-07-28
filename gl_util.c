#include "read_file.h"
#include "gl_util.h"
#include <stdio.h>
#include <stdlib.h>

GLuint compile_shader(GLenum type, char *shader_file_path) {
    char *source;
    if (!read_file_to_str(&source, shader_file_path)) {
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
