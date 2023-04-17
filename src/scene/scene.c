#define STB_PERLIN_IMPLEMENTATION

#include "scene.h"
#include "api.h"
#include "gl.h"
#include "rand.h"
#include "resources.h"
#include "stb_perlin.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef MONOLITH
#define VAR_u_InputSampler "u_InputSampler"
#define VAR_u_PerlinSampler "u_PerlinSampler"
#define VAR_u_RandSampler "u_RandSampler"
#define VAR_u_Brightness "u_Brightness"
#define VAR_u_NoiseScale "u_NoiseScale"
#endif

typedef struct tracks_t_ {
    const void *brightness;
    const void *noisetime;
} tracks_t;

void tracks_init(tracks_t *tracks, gettrack_t gettrack) {
    tracks->brightness = gettrack("post:brightness");
    tracks->noisetime = gettrack("post:noisetime");
}

#define NOISE_SIZE 256

typedef struct post_t_ {
    GLsizei width;
    GLsizei height;
    GLuint fbo;
    GLuint fbo_texture;
    GLuint program;
    GLuint buffer;
    GLuint vao;
    GLuint noise[2];
    void *noise_buffer;
} post_t;

static const GLfloat quad[] = {-1.f, -1., 0., 0., 0., 1.,  -1., 0., 1., 0.,
                               1.,   1.,  0., 1., 1., -1., -1., 0., 0., 0.,
                               1.,   1.,  0., 1., 1., -1., 1.,  0., 0., 1.};

void post_init(post_t *post, GLsizei width, GLsizei height,
               GLuint vertex_shader) {
    post->width = width;
    post->height = height;

    // create post->fbo
    glGenFramebuffers(1, &post->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, post->fbo);
    glGenTextures(1, &post->fbo_texture);
    glBindTexture(GL_TEXTURE_2D, post->fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           post->fbo_texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "FBO not complete\n");
    }

    // load post fragment shader
    GLuint post_shader = SHADER(GL_FRAGMENT_SHADER, post, frag);

    // create post program
    post->program = link_program(2, (GLuint[]){vertex_shader, post_shader});

    // create buffer and va for post quad
    glGenBuffers(1, &post->buffer);
    glGenVertexArrays(1, &post->vao);
    glBindVertexArray(post->vao);
    glBindBuffer(GL_ARRAY_BUFFER, post->buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 30, (const GLvoid *)quad,
                 GL_STATIC_DRAW);
    size_t stride = sizeof(GLfloat) * 5;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (const void *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);

    // Create textures and buffer for noise
    glGenTextures(2, post->noise);
    glBindTexture(GL_TEXTURE_2D, post->noise[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, NOISE_SIZE, NOISE_SIZE, 0, GL_RED,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, post->noise[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, NOISE_SIZE, NOISE_SIZE, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Enough for rgba 8-bit and r 32-bit
    post->noise_buffer = malloc(NOISE_SIZE * NOISE_SIZE * 4);
}

void post_deinit(post_t *post) { free(post->noise_buffer); }

void post_draw(post_t *post, const tracks_t *tr, getval_t get_value) {
    // Create perlin noise
    float z = get_value(tr->noisetime);
    float *perlin = post->noise_buffer;
    for (GLsizei i = 0; i < NOISE_SIZE * NOISE_SIZE; i++) {
        float y = i / (float)NOISE_SIZE;
        float x = i % NOISE_SIZE;
        x /= 4.;
        y /= 4.;
        perlin[i] =
            stb_perlin_noise3(x, y, z, NOISE_SIZE / 4, NOISE_SIZE / 4, 0);
    }
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, post->noise[0]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, NOISE_SIZE, NOISE_SIZE, GL_RED,
                    GL_FLOAT, perlin);

    // Create rand noise
    unsigned char *noise = post->noise_buffer;
    for (GLsizei i = 0; i < NOISE_SIZE * NOISE_SIZE * 4; i++) {
        noise[i] = rand_xoshiro();
    }
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, post->noise[1]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, NOISE_SIZE, NOISE_SIZE, GL_RGBA,
                    GL_UNSIGNED_BYTE, noise);

    // Bind other stuff
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, post->fbo_texture);

    glUseProgram(post->program);
    glUniform1i(glGetUniformLocation(post->program, VAR_u_InputSampler), 0);
    glUniform1i(glGetUniformLocation(post->program, VAR_u_PerlinSampler), 1);
    glUniform1i(glGetUniformLocation(post->program, VAR_u_RandSampler), 2);
    glUniform1f(glGetUniformLocation(post->program, VAR_u_Brightness),
                get_value(tr->brightness));
    glUniform2f(glGetUniformLocation(post->program, VAR_u_NoiseScale),
                post->width / (float)NOISE_SIZE,
                post->height / (float)NOISE_SIZE);
    glBindVertexArray(post->vao);

    // Draw screen quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

typedef struct scene_t_ {
    getval_t get_value;
    gettrack_t get_track;
    tracks_t tr;
    post_t post;
    int32_t width;
    int32_t height;
    GLuint program;
    GLuint buffer;
    GLuint vao;
} scene_t;

static const GLfloat triangle[] = {-0.5f, -0.5, 0.0, 0.5, -0.5,
                                   0.0,   0.0,  0.5, 0.0};
void *scene_init(int32_t width, int32_t height, gettrack_t gettrack,
                 getval_t getval) {
#ifdef __MINGW64__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
#endif

    // set up viewport
    glViewport(0, 0, width, height);

    scene_t *scene = malloc(sizeof(scene_t));
    scene->get_value = getval;
    scene->get_track = gettrack;
    scene->width = width;
    scene->height = height;
    tracks_init(&scene->tr, gettrack);

    // load vertex shader
    GLuint vertex_shader = SHADER(GL_VERTEX_SHADER, shader, vert);

    // load basic fragment shader
    GLuint fragment_shader = SHADER(GL_FRAGMENT_SHADER, shader, frag);

    // create basic program
    scene->program =
        link_program(2, (GLuint[]){vertex_shader, fragment_shader});

    // create buffer and va for hello triangle
    glGenBuffers(1, &scene->buffer);
    glGenVertexArrays(1, &scene->vao);
    glBindVertexArray(scene->vao);
    glBindBuffer(GL_ARRAY_BUFFER, scene->buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9, (const GLvoid *)triangle,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    post_init(&scene->post, width, height, vertex_shader);

    return scene;
}

void scene_deinit(void *data) {
    scene_t *scene = data;
    post_deinit(&scene->post);
    free(scene);
}

void scene_render(double time, void *data) {
    scene_t *scene = data;

    // draw test triangle
    glBindFramebuffer(GL_FRAMEBUFFER, scene->post.fbo);
    glClearColor(sin(time), 1., 0., 1.);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(scene->program);
    glBindVertexArray(scene->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // draw post pass
    post_draw(&scene->post, &scene->tr, scene->get_value);

    // use MESA_DEBUG=1 env to debug
}
