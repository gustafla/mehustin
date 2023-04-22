#include "scene.h"
#include "api.h"
#include "gl.h"
#include "post.h"
#include "resources.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void tracks_init(tracks_t *tracks, gettrack_t gettrack) {
    tracks->brightness = gettrack("post:brightness");
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

void scene_resize(void *data, uint32_t width, uint32_t height) {
    scene_t *scene = data;
    post_resize(&scene->post, width, height);
}

void scene_deinit(void *data) {
    if (data) {
        scene_t *scene = data;
        post_deinit(&scene->post);
        free(scene);
    }
}

void scene_render(void *data, double time) {
    scene_t *scene = data;
    glViewport(0, 0, scene->width, scene->height);

    // draw test triangle
    glClearColor(sin(time), 1., 0., 1.);
    post_bind_fbo(&scene->post);

    glUseProgram(scene->program);
    glBindVertexArray(scene->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // draw post pass
    post_draw(&scene->post, &scene->tr, scene->get_value);

    // use MESA_DEBUG=1 env to debug
}
