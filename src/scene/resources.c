#include "gl.h"
#include <stdio.h>
#include <stdlib.h>

size_t read_file_to_str(const char *file_path, char **dst) {
    FILE *file = fopen(file_path, "r");
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

GLuint compile_shader(GLenum shader_type, const char *shader_src) {
    GLuint shader;

    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, (const GLchar *const *)&shader_src, NULL);
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

GLuint compile_shader_file(GLenum shader_type, const char *path) {
    char *shader_src = NULL;

    if (read_file_to_str(path, &shader_src) == 0) {
        return 0;
    }

    GLuint shader = compile_shader(shader_type, shader_src);
    free(shader_src);

    if (shader == 0) {
        fprintf(stderr, "File: %s\n", path);
    }

    return shader;
}
