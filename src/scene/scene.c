#include "scene.h"
#include "read_file.h"
#include <GLES2/gl2.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef MONOLITH
#include "shader_code.h"
#else
#define VAR_a_Pos "a_Pos"
#define VAR_a_TexCoord "a_TexCoord"
#define VAR_u_InputSampler "u_InputSampler"
#define VAR_v_Pos "v_Pos"
#define VAR_v_TexCoord "v_TexCoord"
#endif

typedef struct scene_t_ {
    double (*get_value)(const void *);
    const void *(*get_track)(const char *);
    int32_t width;
    int32_t height;
    GLuint program;
    GLuint buffer;
    GLuint post_fbo;
    GLuint post_fbo_texture;
    GLuint post_program;
    GLuint post_buffer;
} scene_t;

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

#ifdef MONOLITH
#define SHADER(GLTYPE, NAME, TYPE) compile_shader(GLTYPE, NAME##_##TYPE)
#else
#define SHADER(GLTYPE, NAME, TYPE) compile_shader_file(GLTYPE, #NAME "." #TYPE)
#endif

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

void *scene_init(int32_t width, int32_t height,
                 const void *(*gettrack)(const char *),
                 double (*getval)(const void *)) {
    // set up viewport
    glViewport(0, 0, width, height);

    scene_t *scene = malloc(sizeof(scene_t));
    scene->get_value = getval;
    scene->get_track = gettrack;
    scene->width = width;
    scene->height = height;

    // load vertex shader
    GLuint vertex_shader = SHADER(GL_VERTEX_SHADER, shader, vert);

    // load basic fragment shader
    GLuint fragment_shader = SHADER(GL_FRAGMENT_SHADER, shader, frag);

    // load post fragment shader
    GLuint post_shader = SHADER(GL_FRAGMENT_SHADER, post, frag);

    // create basic program
    scene->program =
        link_program(2, (GLuint[]){vertex_shader, fragment_shader});

    // create post program
    scene->post_program =
        link_program(2, (GLuint[]){vertex_shader, post_shader});

    // create buffer for hello triangle
    float triangle[] = {-0.5f, -0.5, 0.0, 0.5, -0.5, 0.0, 0.0, 0.5, 0.0};
    glGenBuffers(1, &scene->buffer);
    glBindBuffer(GL_ARRAY_BUFFER, scene->buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, (const GLvoid *)triangle,
                 GL_STATIC_DRAW);

    // create buffer for post quad
    float quad[] = {-1.f, -1., 0., 0., 0., 1.,  -1., 0., 1., 0.,
                    1.,   1.,  0., 1., 1., -1., -1., 0., 0., 0.,
                    1.,   1.,  0., 1., 1., -1., 1.,  0., 0., 1.};
    glGenBuffers(1, &scene->post_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, scene->post_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30, (const GLvoid *)quad,
                 GL_STATIC_DRAW);

    // create post fbo
    glGenFramebuffers(1, &scene->post_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, scene->post_fbo);
    glGenTextures(1, &scene->post_fbo_texture);
    glBindTexture(GL_TEXTURE_2D, scene->post_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           scene->post_fbo_texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "FBO not complete\n");
    }

    return scene;
}

void scene_deinit(void *data) { free(data); }

void scene_render(double time, void *data) {
    GLint pos_attrib, tex_coord_attrib;
    scene_t *scene = data;

    // draw test triangle
    glBindFramebuffer(GL_FRAMEBUFFER, scene->post_fbo);
    glClearColor(sin(time), 1., 0., 1.);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, scene->buffer);
    pos_attrib = glGetAttribLocation(scene->program, VAR_a_Pos);
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glUseProgram(scene->program);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // draw post pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, scene->post_fbo_texture);

    glBindBuffer(GL_ARRAY_BUFFER, scene->post_buffer);
    pos_attrib = glGetAttribLocation(scene->post_program, VAR_a_Pos);
    tex_coord_attrib = glGetAttribLocation(scene->post_program, VAR_a_TexCoord);
    size_t stride = sizeof(float) * 5;
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, stride, NULL);
    glEnableVertexAttribArray(tex_coord_attrib);
    glVertexAttribPointer(tex_coord_attrib, 2, GL_FLOAT, GL_FALSE, stride,
                          (const void *)(sizeof(float) * 3));

    glUseProgram(scene->post_program);
    glUniform1i(glGetUniformLocation(scene->post_program, VAR_u_InputSampler),
                0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

#ifdef DEBUG
#define CASE(VAL) fprintf(stderr, #VAL "\n");
    // check opengl errors
    switch (glGetError()) {
    case GL_NO_ERROR:
        break;
    case GL_INVALID_ENUM:
        CASE(GL_INVALID_ENUM) break;
    case GL_INVALID_VALUE:
        CASE(GL_INVALID_VALUE) break;
    case GL_INVALID_OPERATION:
        CASE(GL_INVALID_OPERATION) break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        CASE(GL_INVALID_FRAMEBUFFER_OPERATION) break;
    case GL_OUT_OF_MEMORY:
        CASE(GL_OUT_OF_MEMORY) break;
    default:
        fprintf(stderr, "Unknown glGetError() return value\n");
    }
#endif
}
