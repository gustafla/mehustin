#include "gl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t read_file_to_str(const char *file_path, char **dst) {
    FILE *file = fopen(file_path, "rb");
    if (!file)
        goto error;

    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    if (!len)
        goto error;
    *dst = (char *)malloc(len + 1);
    fseek(file, 0, SEEK_SET);

    size_t read = fread(*dst, sizeof(char), len, file);
    fclose(file);
    if (read != len)
        goto cleanup;

    (*dst)[len] = '\0';

    return len;

cleanup:
    free(*dst);
    *dst = NULL;
error:
    fprintf(stderr, "Failed to read file %s\n", file_path);
    return 0;
}

static const char *SHADER_VERSION = "#version 330 core\n";

static void shader_source(GLuint shader, const char *src, const char *defines) {
    const char *srcs[] = {SHADER_VERSION, src, src};
    if (defines) {
        srcs[1] = defines;
    }
    glShaderSource(shader, 2 + (defines ? 1 : 0), srcs, NULL);
}

static GLenum type_to_enum(const char *shader_type) {
    if (strcmp("vert", shader_type) == 0) {
        return GL_VERTEX_SHADER;
    } else if (strcmp("geom", shader_type) == 0) {
        return GL_GEOMETRY_SHADER;
    } else if (strcmp("frag", shader_type) == 0) {
        return GL_FRAGMENT_SHADER;
    }
    fprintf(stderr, "Unrecognized shader type: %s\n", shader_type);
    return GL_INVALID_ENUM;
}

GLuint compile_shader(const char *shader_src, const char *shader_type,
                      const char *defines) {
    GLuint shader;

    shader = glCreateShader(type_to_enum(shader_type));
    shader_source(shader, shader_src, defines);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint log_len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
        GLchar *log = malloc(sizeof(GLchar) * log_len);
        glGetShaderInfoLog(shader, log_len, NULL, log);
        fprintf(stderr, "Shader compilation failed:\n%s\n", log);
        free(log);
        return 0;
    }

    return shader;
}

GLuint compile_shader_file(const char *filename, const char *defines) {
    char *shader_src = NULL;

    if (read_file_to_str(filename, &shader_src) == 0) {
        return 0;
    }

    // Find file extension
    const char *shader_type = filename, *ret;
    do {
        if ((ret = strchr(shader_type, '.'))) {
            shader_type = ret + 1;
        }
    } while (ret);

    GLuint shader = compile_shader(shader_src, shader_type, defines);
    free(shader_src);

    if (shader == 0) {
        fprintf(stderr, "File: %s\n", filename);
    }

    return shader;
}

GLuint link_program(size_t count, GLuint *shaders) {
    GLuint program = glCreateProgram();

    for (size_t i = 0; i < count; i++) {
        glAttachShader(program, shaders[i]);
    }

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint log_len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
        GLchar *log = malloc(sizeof(GLchar) * log_len);
        glGetProgramInfoLog(program, log_len, NULL, log);
        fprintf(stderr, "Program linking failed.\n%s\n", log);
        free(log);
        return 0;
    }

    return program;
}
