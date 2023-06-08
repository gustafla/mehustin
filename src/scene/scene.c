#include "scene.h"
#include "api.h"
#include "cglm/cam.h"
#include "gl.h"
#include "post.h"
#include "primitives.h"
#include "rand.h"
#include "resources.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POINTS 4096

void tracks_init(tracks_t *tracks, gettrack_t gettrack) {
    tracks->brightness = gettrack("post:brightness");
    tracks->cam_pos_x = gettrack("cam:pos.x");
    tracks->cam_pos_y = gettrack("cam:pos.y");
    tracks->cam_pos_z = gettrack("cam:pos.z");
    tracks->cam_dir_x = gettrack("cam:dir.x");
    tracks->cam_dir_y = gettrack("cam:dir.y");
    tracks->cam_dir_z = gettrack("cam:dir.z");
    tracks->cam_focus = gettrack("cam:focus");
}

#define GET_CAM_POS                                                            \
    (vec3) {                                                                   \
        scene->get_value(scene->tr.cam_pos_x),                                 \
            scene->get_value(scene->tr.cam_pos_y),                             \
            scene->get_value(scene->tr.cam_pos_z),                             \
    }

#define GET_CAM_DIR                                                            \
    (vec3) {                                                                   \
        scene->get_value(scene->tr.cam_dir_x),                                 \
            scene->get_value(scene->tr.cam_dir_y),                             \
            scene->get_value(scene->tr.cam_dir_z),                             \
    }

typedef struct scene_t_ {
    getval_t get_value;
    gettrack_t get_track;
    tracks_t tr;
    primitives_t primitives;
    post_t post;
    int32_t width;
    int32_t height;
    GLuint program;
    GLuint point_instance_buffer;
    GLuint vao;
    mat4 view;
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

    // load rendering primitives
    primitives_init(&scene->primitives);

    // load vertex shader
    GLuint vertex_shader = SHADER(point, vert, NULL);

    // load fragment shader
    GLuint fragment_shader = SHADER(shader, frag, NULL);

    // create basic program
    scene->program =
        link_program(2, (GLuint[]){vertex_shader, fragment_shader});

    // create instances for points
    vec3 point_instances[POINTS] = {0};
    for (int i = 0; i < POINTS; i++) {
        random_vec3(point_instances[i]);
    }
    glGenBuffers(1, &scene->point_instance_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, scene->point_instance_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point_instances),
                 (const GLvoid *)point_instances, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // set up vertex attributes -----------------------------------------------
    glGenVertexArrays(1, &scene->vao);
    glBindVertexArray(scene->vao);

    // 1. quad positions and texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, scene->primitives.quad_vertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5,
                          (const void *)(sizeof(GLfloat) * 3));

    // 2. quad instance world positions
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, scene->point_instance_buffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);
    // ------------------------------------------------------------------------

    // projection matrix
    float aspect = (float)width / (float)height;
    glm_perspective(90., aspect, 0.1, 100., scene->projection);

    post_init(&scene->post, &scene->primitives, width, height);

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
        primitives_deinit(&scene->primitives);
        free(scene);
    }
}

void scene_render(void *data, double time) {
    scene_t *scene = data;
    glViewport(0, 0, scene->width, scene->height);
    glDisable(GL_DEPTH_TEST);

    // draw point cube
    glClearColor(0., 0, 0., 1.);
    pass_fbo_bind(post_get_fbo(&scene->post));
    glEnablei(GL_BLEND, post_get_fbo(&scene->post)->fbo);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    glUseProgram(scene->program);
    glm_look_anyup(GET_CAM_POS, GET_CAM_DIR, scene->view);
    glUniformMatrix4fv(glGetUniformLocation(scene->program, VAR_u_View), 1,
                       GL_FALSE, (float *)scene->view);
    glUniformMatrix4fv(glGetUniformLocation(scene->program, VAR_u_Projection),
                       1, GL_FALSE, (float *)scene->projection);
    glUniform1f(glGetUniformLocation(scene->program, VAR_u_Focus),
                scene->get_value(scene->tr.cam_focus));
    glBindVertexArray(scene->vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, POINTS);
    glBindVertexArray(0);

    // draw post pass
    post_draw(&scene->post, &scene->tr, scene->get_value);

    // use MESA_DEBUG=1 env to debug
}
