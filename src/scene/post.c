#define STB_PERLIN_IMPLEMENTATION
#include "post.h"
#include "rand.h"
#include "resources.h"
#include "stb_perlin.h"
#include <stdlib.h>

void viewport_bind(const viewport_t *v) {
    glViewport(v->x, v->y, v->width, v->height);
}

void viewport_set_u_resolution(const viewport_t *v, GLuint location) {
    glUniform2f(location, v->width, v->height);
}

void post_init(post_t *post, primitives_t *primitives, GLsizei width,
               GLsizei height) {
    post->source_aspect_ratio = (float)width / (float)height;
    post->source_viewport.x = 0;
    post->source_viewport.y = 0;
    post->source_viewport.width = width;
    post->source_viewport.height = height;
    // Initially the same as source, changes by calls to post_resize
    post->output_viewport = post->source_viewport;

    // create post->fbos
    for (int i = 0; i < N_FBOS; i++) {
        pass_fbo_init(&post->fbos[i], width, height);
    }

    // load vertex shader
    GLuint vertex_shader = SHADER(shader, vert, NULL);

    // create shader passes
    pass_init(&post->bloom_pre, vertex_shader, SHADER(bloom_pre, frag, NULL));
    pass_init(&post->blurx, vertex_shader,
              SHADER(blur, frag, "#define HORIZONTAL\n"));
    pass_init(&post->blury, vertex_shader, SHADER(blur, frag, NULL));
    pass_init(&post->pass, vertex_shader, SHADER(post, frag, NULL));

    // create va for post quad
    glGenVertexArrays(1, &post->vao);
    glBindVertexArray(post->vao);
    glBindBuffer(GL_ARRAY_BUFFER, primitives->quad_vertices);
    size_t stride = sizeof(GLfloat) * 5;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (const void *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);

    // Create textures and buffer for noise
    // Enough for rgba 8-bit and r 32-bit
    post->noise_buffer = malloc(NOISE_SIZE * NOISE_SIZE * 4);
    float z = 0.4;
    float *perlin = post->noise_buffer;
    for (GLsizei i = 0; i < NOISE_SIZE * NOISE_SIZE; i++) {
        float y = i / (float)NOISE_SIZE;
        float x = i % NOISE_SIZE;
        x /= 4.;
        y /= 4.;
        perlin[i] =
            stb_perlin_noise3(x, y, z, NOISE_SIZE / 4, NOISE_SIZE / 4, 0);
    }
    glGenTextures(2, post->noise);
    glBindTexture(GL_TEXTURE_2D, post->noise[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, NOISE_SIZE, NOISE_SIZE, 0, GL_RED,
                 GL_FLOAT, perlin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, post->noise[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, NOISE_SIZE, NOISE_SIZE, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void post_deinit(post_t *post) {
    if (post) {
        free(post->noise_buffer);
        for (int i = 0; i < N_FBOS; i++) {
            pass_fbo_deinit(&post->fbos[i]);
        }
        pass_deinit(&post->bloom_pre);
        pass_deinit(&post->blurx);
        pass_deinit(&post->blury);
        pass_deinit(&post->pass);
    }
}

void post_draw(post_t *post, gettrack_t get_track, getval_t get_value) {
    glClearColor(0, 0, 0, 1.);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // Start pass to bloom_pre buffer -----------------------------------------

    pass_fbo_bind(&post->fbos[1]);
    viewport_bind(&post->source_viewport);
    pass_bind(&post->bloom_pre);
    pass_fbo_bind_tex(&post->fbos[0], 0); // Main image
    glUniform1i(pass_ufmloc(&post->bloom_pre, VAR_u_InputSampler), 0);
    viewport_set_u_resolution(&post->source_viewport,
                              pass_ufmloc(&post->bloom_pre, VAR_u_Resolution));
    pass_draw(post->vao);

    // Start pass to blur_x buffer --------------------------------------------

    pass_fbo_bind(&post->fbos[2]);
    viewport_bind(&post->source_viewport);
    pass_bind(&post->blurx);
    pass_fbo_bind_tex(&post->fbos[1], 0); // Bloom pre image
    glUniform1i(pass_ufmloc(&post->blurx, VAR_u_InputSampler), 0);
    viewport_set_u_resolution(&post->source_viewport,
                              pass_ufmloc(&post->blurx, VAR_u_Resolution));
    pass_draw(post->vao);

    // Start pass to blur_y buffer --------------------------------------------

    pass_fbo_bind(&post->fbos[1]);
    viewport_bind(&post->source_viewport);
    pass_bind(&post->blury);
    pass_fbo_bind_tex(&post->fbos[2], 0); // Bloom X image
    glUniform1i(pass_ufmloc(&post->blury, VAR_u_InputSampler), 0);
    viewport_set_u_resolution(&post->source_viewport,
                              pass_ufmloc(&post->blury, VAR_u_Resolution));
    pass_draw(post->vao);

    // Start pass to output back buffer ---------------------------------------
    pass_fbo_bind(NULL);
    viewport_bind(&post->output_viewport);
    pass_bind(&post->pass);

    // Bind earlier pass' outputs as inputs
    pass_fbo_bind_tex(&post->fbos[0], 0); // Main image
    pass_fbo_bind_tex(&post->fbos[1], 1); // Bloom

    // Bind perlin noise
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, post->noise[0]);

    // Create rand noise
    unsigned char *noise = post->noise_buffer;
    for (GLsizei i = 0; i < NOISE_SIZE * NOISE_SIZE * 4; i++) {
        noise[i] = rand_xoshiro();
    }
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, post->noise[1]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, NOISE_SIZE, NOISE_SIZE, GL_RGBA,
                    GL_UNSIGNED_BYTE, noise);

    glUniform1i(pass_ufmloc(&post->pass, VAR_u_InputSampler), 0);
    glUniform1i(pass_ufmloc(&post->pass, VAR_u_BloomSampler), 1);
    glUniform1i(pass_ufmloc(&post->pass, VAR_u_PerlinSampler), 2);
    glUniform1i(pass_ufmloc(&post->pass, VAR_u_RandSampler), 3);
    glUniform1f(pass_ufmloc(&post->pass, VAR_u_Brightness),
                get_value(get_track("post:brightness")));
    glUniform1i(pass_ufmloc(&post->pass, VAR_u_NoiseSize), NOISE_SIZE);
    viewport_set_u_resolution(&post->output_viewport,
                              pass_ufmloc(&post->pass, VAR_u_Resolution));

    // Draw screen quad
    pass_draw(post->vao);
}

void post_resize(post_t *post, uint32_t width, uint32_t height) {
    if ((float)width / (float)height > post->source_aspect_ratio) {
        // new window has greater width than internal size
        post->output_viewport.height = height;
        post->output_viewport.width = height * post->source_aspect_ratio;
        post->output_viewport.y = 0;
        post->output_viewport.x = (width - post->output_viewport.width) / 2;
    } else {
        // new window has greater height than internal size
        post->output_viewport.width = width;
        post->output_viewport.height = width / post->source_aspect_ratio;
        post->output_viewport.x = 0;
        post->output_viewport.y = (height - post->output_viewport.height) / 2;
    }
}

const pass_fbo_t *post_get_fbo(const post_t *post) { return &post->fbos[0]; }
