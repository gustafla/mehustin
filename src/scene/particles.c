#include "particles.h"
#include "api.h"
#include "cglm/vec3.h"
#include "rand.h"
#include "resources.h"
#include "scene/scene.h"
#include "stb_perlin.h"

static float random_float(void) {
    uint16_t random = rand_xoshiro();
    return ((float)random / (float)UINT16_MAX) * 2. - 1.;
}

static void random_vec3(vec3 vec) {
    vec[0] = random_float();
    vec[1] = random_float();
    vec[2] = random_float();
}

void particles_init(particles_t *particles, primitives_t *const primitives) {
    // load vertex shader
    GLuint vertex_shader = SHADER(point, vert, NULL);

    // load fragment shader
    GLuint fragment_shader = SHADER(point, frag, NULL);

    // create basic program
    particles->program =
        link_program(2, (GLuint[]){vertex_shader, fragment_shader});

    // create instances for points
    vec3 point_instances[POINTS] = {0};
    for (int i = 0; i < POINTS; i++) {
        random_vec3(point_instances[i]);
    }
    particles->particle_velocity = calloc(POINTS, sizeof(float) * 3);

    glGenBuffers(1, &particles->point_instance_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles->point_instance_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * POINTS,
                 (const GLvoid *)point_instances, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // set up vertex attributes -----------------------------------------------
    glGenVertexArrays(1, &particles->vao);
    glBindVertexArray(particles->vao);

    // 1. quad positions and texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, primitives->quad_vertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5,
                          (const void *)(sizeof(GLfloat) * 3));

    // 2. quad instance world positions
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles->point_instance_buffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);
}

void particles_deinit(particles_t *particles) {
    glDeleteBuffers(1, &particles->point_instance_buffer);
    free(particles->particle_velocity);
}

void particles_simulate_step(particles_t *particles, float delta_time) {
    glBindBuffer(GL_ARRAY_BUFFER, particles->point_instance_buffer);
    float *point_instances = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

    // Integrate velocity
    for (size_t i = 0; i < POINTS * 3; i += 3) {
        particles->particle_velocity[i] +=
            stb_perlin_noise3_seed(point_instances[i], point_instances[i + 1],
                                   point_instances[i + 2], 0, 0, 0, 23) *
            delta_time;
        particles->particle_velocity[i + 1] +=
            stb_perlin_noise3_seed(point_instances[i], point_instances[i + 1],
                                   point_instances[i + 2], 0, 0, 0, 3) *
            delta_time;
        particles->particle_velocity[i + 2] +=
            stb_perlin_noise3_seed(point_instances[i], point_instances[i + 1],
                                   point_instances[i + 2], 0, 0, 0, 12) *
            delta_time;
    }

    // Integrate position
    for (size_t i = 0; i < POINTS * 3; i += 3) {
        point_instances[i] += particles->particle_velocity[i] * delta_time;
        point_instances[i + 1] +=
            particles->particle_velocity[i + 1] * delta_time;
        point_instances[i + 2] +=
            particles->particle_velocity[i + 2] * delta_time;
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void particles_draw(particles_t *particles, gettrack_t get_track,
                    getval_t get_value, mat4 view, mat4 projection) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    glUseProgram(particles->program);
    glUniformMatrix4fv(glGetUniformLocation(particles->program, VAR_u_View), 1,
                       GL_FALSE, (float *)view);
    glUniformMatrix4fv(
        glGetUniformLocation(particles->program, VAR_u_Projection), 1, GL_FALSE,
        (float *)projection);
    glUniform1f(glGetUniformLocation(particles->program, VAR_u_Focus),
                get_value(get_track("cam:focus")));

    glBindVertexArray(particles->vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, POINTS);
    glBindVertexArray(0);
}
