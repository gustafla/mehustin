#include "scene.h"
#include "api.h"
#include "cglm/cam.h"
#include "gl.h"
#include "post.h"
#include "rand.h"
#include "resources.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POINTS 4096

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
    GLuint buffers[1];
    GLuint vao;
    mat4 projection;
} scene_t;

float random_float(void) {
    uint16_t random = rand_xoshiro();
    return ((float)random / (float)UINT16_MAX) * 2. - 1.;
}

void random_vec3(vec3 vec) {
    vec[0] = random_float();
    vec[1] = random_float();
    vec[2] = random_float();
}

void *scene_init(int32_t width, int32_t height, gettrack_t gettrack,
                 getval_t getval) {
#ifdef __MINGW64__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
#endif

    scene_t *scene = malloc(sizeof(scene_t));
    scene->get_value = getval;
    scene->get_track = gettrack;
    scene->width = width;
    scene->height = height;
    tracks_init(&scene->tr, gettrack);

    // load vertex shader
    GLuint vertex_shader = SHADER(point, vert, NULL);

    // load fragment shader
    GLuint fragment_shader = SHADER(shader, frag, NULL);

    // create basic program
    scene->program =
        link_program(2, (GLuint[]){vertex_shader, fragment_shader});

    vec3 point_vertices[POINTS * 3 * 2] = {0};
    for (int i = 0; i < POINTS; i++) {
        vec3 vertex;
        random_vec3(vertex);
        for (int j = 0; j < 6; j++) {
            memcpy(point_vertices[i * 6 + j], vertex, sizeof(vertex));
        }
    }

    // create buffer and va for points
    glGenBuffers(1, scene->buffers);
    glGenVertexArrays(1, &scene->vao);
    glBindVertexArray(scene->vao);
    glBindBuffer(GL_ARRAY_BUFFER, scene->buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point_vertices),
                 (const GLvoid *)point_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // projection matrix
    float aspect = (float)width / (float)height;
    glm_perspective(90., aspect, 0.1, 100., scene->projection);

    post_init(&scene->post, width, height);

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
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // draw point cube
    glClearColor(0., 0, 0., 1.);
    post_bind_fbo(&scene->post);

    glUseProgram(scene->program);
    glUniformMatrix4fv(glGetUniformLocation(scene->program, VAR_u_Projection),
                       1, GL_FALSE, (float *)scene->projection);
    glBindVertexArray(scene->vao);
    glDrawArrays(GL_TRIANGLES, 0, POINTS * 6);

    // draw post pass
    post_draw(&scene->post, &scene->tr, scene->get_value);

    // use MESA_DEBUG=1 env to debug
}
