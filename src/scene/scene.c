#include "scene.h"
#include "api.h"
#include "cglm/cam.h"
#include "gl.h"
#include "post.h"
#include "primitives.h"
#include "resources.h"
#include "scene/particles.h"
#include "text.h"
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GET_CAM_POS                                                            \
    (vec3) {                                                                   \
        scene->get_value(scene->get_track("cam:pos.x")),                       \
            scene->get_value(scene->get_track("cam:pos.y")),                   \
            scene->get_value(scene->get_track("cam:pos.z")),                   \
    }

#define GET_CAM_DIR                                                            \
    (vec3) {                                                                   \
        scene->get_value(scene->get_track("cam:dir.x")),                       \
            scene->get_value(scene->get_track("cam:dir.y")),                   \
            scene->get_value(scene->get_track("cam:dir.z")),                   \
    }

typedef struct scene_t_ {
    getval_t get_value;
    gettrack_t get_track;
    double t_1;
    primitives_t primitives;
    post_t post;
    int32_t width;
    int32_t height;
    mat4 view;
    mat4 projection;
    particles_t particles;
    text_t group_text;
} scene_t;

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

    // load rendering primitives
    primitives_init(&scene->primitives);
    // ------------------------------------------------------------------------

    // projection matrix
    float aspect = (float)width / (float)height;
    glm_perspective(90., aspect, 0.1, 100., scene->projection);

    post_init(&scene->post, &scene->primitives, width, height);

    particles_init(&scene->particles, &scene->primitives);

    text_init(&scene->group_text, "Mehu", 128, 0.4);

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
        particles_deinit(&scene->particles);
        free(scene);
    }
}

void scene_render(void *data, double time) {
    scene_t *scene = data;

    double delta_time = time - scene->t_1;

    glViewport(0, 0, scene->width, scene->height);
    glm_look_anyup(GET_CAM_POS, GET_CAM_DIR, scene->view);
    glClearColor(0., 0, 0., 1.);
    pass_fbo_bind(post_get_fbo(&scene->post));

    // draw point cube
    particles_simulate_step(&scene->particles, delta_time * 0.1);
    particles_draw(&scene->particles, scene->get_track, scene->get_value,
                   scene->view, scene->projection);

    text_draw(&scene->group_text, scene->view, scene->projection,
              scene->get_value(scene->get_track("mehu_scale")));

    // draw post pass
    post_draw(&scene->post, scene->get_track, scene->get_value);

    // use MESA_DEBUG=1 env to debug
    scene->t_1 = time;
}
