#include "scene.h"
#include "gl.h"
#include "resources.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef MONOLITH
#define VAR_u_InputSampler "u_InputSampler"
#define VAR_u_Brightness "u_Brightness"
#endif

typedef struct scene_t_ {
    double (*get_value)(const void *);
    const void *(*get_track)(const char *);
    const void *tr_brightness;
    int32_t width;
    int32_t height;
    GLuint program;
    GLuint buffer;
    GLuint vao;
    GLuint post_fbo;
    GLuint post_fbo_texture;
    GLuint post_program;
    GLuint post_buffer;
    GLuint post_vao;
} scene_t;

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

static const GLfloat triangle[] = {-0.5f, -0.5, 0.0, 0.5, -0.5,
                                   0.0,   0.0,  0.5, 0.0};
static const GLfloat quad[] = {-1.f, -1., 0., 0., 0., 1.,  -1., 0., 1., 0.,
                               1.,   1.,  0., 1., 1., -1., -1., 0., 0., 0.,
                               1.,   1.,  0., 1., 1., -1., 1.,  0., 0., 1.};

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

    // brightness track
    scene->tr_brightness = gettrack("post:brightness");

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

    // create buffer and va for hello triangle
    glGenBuffers(1, &scene->buffer);
    glGenVertexArrays(1, &scene->vao);
    glBindVertexArray(scene->vao);
    glBindBuffer(GL_ARRAY_BUFFER, scene->buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9, (const GLvoid *)triangle,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // create buffer and va for post quad
    glGenBuffers(1, &scene->post_buffer);
    glGenVertexArrays(1, &scene->post_vao);
    glBindVertexArray(scene->post_vao);
    glBindBuffer(GL_ARRAY_BUFFER, scene->post_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 30, (const GLvoid *)quad,
                 GL_STATIC_DRAW);
    size_t stride = sizeof(GLfloat) * 5;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (const void *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);

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
    scene_t *scene = data;

    // draw test triangle
    glBindFramebuffer(GL_FRAMEBUFFER, scene->post_fbo);
    glClearColor(sin(time), 1., 0., 1.);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(scene->program);
    glBindVertexArray(scene->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // draw post pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, scene->post_fbo_texture);

    glUseProgram(scene->post_program);
    glUniform1i(glGetUniformLocation(scene->post_program, VAR_u_InputSampler),
                0);
    glUniform1f(glGetUniformLocation(scene->post_program, VAR_u_Brightness),
                scene->get_value(scene->tr_brightness));
    glBindVertexArray(scene->post_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // use MESA_DEBUG=1 env to debug
}
