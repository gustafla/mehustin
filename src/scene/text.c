#include "text.h"
#include "cglm/affine.h"
#include "resources.h"
#include "stdlib.h"
#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define ATLAS_SIZE 2048
#define FONT_HEIGHT 360

static GLuint make_depth_instace_buffer(size_t depth, float scale) {
    float *depths = malloc(depth * sizeof(float));
    for (size_t i = 0; i < depth; i++) {
        depths[i] = (-(float)i / (float)depth) * FONT_HEIGHT * scale;
    }

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, depth * sizeof(float), (const GLvoid *)depths,
                 GL_STATIC_DRAW);
    free(depths);
    return buffer;
}

void text_init(text_t *text, char *msg, size_t layers, float scale) {
    static unsigned char temp_bitmap[ATLAS_SIZE * ATLAS_SIZE];

    char *ttf = NULL;
    size_t ttf_size = read_file_to_str("OpenSans-Bold.ttf", &ttf);
    assert(ttf_size != 0);

    stbtt_BakeFontBitmap((const unsigned char *)ttf, 0, FONT_HEIGHT,
                         temp_bitmap, ATLAS_SIZE, ATLAS_SIZE, 32, 96,
                         text->cdata); // no guarantee this fits!
    free(ttf);

    glGenTextures(1, &text->ftex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text->ftex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, ATLAS_SIZE, ATLAS_SIZE, 0, GL_RED,
                 GL_UNSIGNED_BYTE, temp_bitmap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    size_t quad_floats = 4 * 6;
    size_t quad_size = quad_floats * sizeof(float);
    size_t msg_len = strlen(msg);
    float *quads = malloc(msg_len * quad_size);

    float x = 0, y = 0;
    for (size_t i = 0; i < msg_len; i++) {
        char c = msg[i];
        if (c >= 32 && c < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(text->cdata, ATLAS_SIZE, ATLAS_SIZE, c - 32, &x,
                               &y, &q,
                               1); // 1=opengl & d3d10+,0=d3d9

            // x0 y0
            quads[i * quad_floats + 16] = quads[i * quad_floats + 0] = q.x0;
            quads[i * quad_floats + 17] = quads[i * quad_floats + 1] = q.y0;
            quads[i * quad_floats + 18] = quads[i * quad_floats + 2] = q.s0;
            quads[i * quad_floats + 19] = quads[i * quad_floats + 3] = q.t0;

            // x1 y1
            quads[i * quad_floats + 12] = quads[i * quad_floats + 4] = q.x1;
            quads[i * quad_floats + 13] = quads[i * quad_floats + 5] = q.y1;
            quads[i * quad_floats + 14] = quads[i * quad_floats + 6] = q.s1;
            quads[i * quad_floats + 15] = quads[i * quad_floats + 7] = q.t1;

            // x0 y1
            quads[i * quad_floats + 8] = q.x0;
            quads[i * quad_floats + 9] = q.y1;
            quads[i * quad_floats + 10] = q.s0;
            quads[i * quad_floats + 11] = q.t1;

            // x1 y0
            quads[i * quad_floats + 20] = q.x1;
            quads[i * quad_floats + 21] = q.y0;
            quads[i * quad_floats + 22] = q.s1;
            quads[i * quad_floats + 23] = q.t0;
        }
    }

    glGenBuffers(1, &text->buffer);
    glBindBuffer(GL_ARRAY_BUFFER, text->buffer);
    glBufferData(GL_ARRAY_BUFFER, msg_len * quad_size, (const GLvoid *)quads,
                 GL_STATIC_DRAW);
    free(quads);

    glGenVertexArrays(1, &text->vao);
    glBindVertexArray(text->vao);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, text->buffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4,
                          (const void *)(sizeof(GLfloat) * 2));

    text->depth = layers;
    text->depth_instance_buffer = make_depth_instace_buffer(text->depth, scale);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, text->depth_instance_buffer);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 1, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);

    text->count = 6 * msg_len;

    // load vertex shader
    GLuint vertex_shader = SHADER(text, vert, NULL);

    // load fragment shader
    GLuint fragment_shader = SHADER(text, frag, NULL);

    // create basic program
    text->program = link_program(2, (GLuint[]){vertex_shader, fragment_shader});
}

void text_draw(text_t *text, mat4 view, mat4 projection, float z_scale) {
    mat4 model;
    float scale = 1. / FONT_HEIGHT;
    glm_scale_make(model, (vec3){scale, -scale, scale * z_scale});

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text->ftex);

    glUseProgram(text->program);
    glUniform1i(glGetUniformLocation(text->program, VAR_u_FontSampler), 0);
    glUniform1i(glGetUniformLocation(text->program, VAR_u_Layers), text->depth);
    glUniformMatrix4fv(glGetUniformLocation(text->program, VAR_u_Model), 1,
                       GL_FALSE, (float *)model);
    glUniformMatrix4fv(glGetUniformLocation(text->program, VAR_u_View), 1,
                       GL_FALSE, (float *)view);
    glUniformMatrix4fv(glGetUniformLocation(text->program, VAR_u_Projection), 1,
                       GL_FALSE, (float *)projection);

    glBindVertexArray(text->vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, text->count, text->depth);
    glBindVertexArray(0);
}
