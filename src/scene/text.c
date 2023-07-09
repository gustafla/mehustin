#include "text.h"
#include "cglm/affine.h"
#include "resources.h"
#include "stdlib.h"
#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

void text_init(text_t *text, char *msg) {
    static unsigned char temp_bitmap[512 * 512];

    char *ttf = NULL;
    size_t ttf_size = read_file_to_str("LiberationSerif-Regular.ttf", &ttf);
    assert(ttf_size != 0);

    stbtt_BakeFontBitmap((const unsigned char *)ttf, 0, 32.0, temp_bitmap, 512,
                         512, 32, 96,
                         text->cdata); // no guarantee this fits!
    free(ttf);

    glGenTextures(1, &text->ftex);
    glBindTexture(GL_TEXTURE_2D, text->ftex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE,
                 temp_bitmap);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    size_t quad_floats = 4 * 6;
    size_t quad_size = quad_floats * sizeof(float);
    size_t msg_len = strlen(msg);
    float *quads = malloc(msg_len * quad_size);

    float x = 0, y = 0;
    size_t i = 0;
    while (*msg) {
        if (*msg >= 32 && *msg < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(text->cdata, 512, 512, *msg - 32, &x, &y, &q,
                               1); // 1=opengl & d3d10+,0=d3d9
            printf("tex %f, %f\n", q.s0, q.t0);
            printf("pos %f, %f\n", q.x0, q.y0);

            printf("tex %f, %f\n", q.s1, q.t0);
            printf("pos %f, %f\n", q.x1, q.y0);

            printf("tex %f, %f\n", q.s1, q.t1);
            printf("pos %f, %f\n", q.x1, q.y1);

            printf("tex %f, %f\n", q.s0, q.t1);
            printf("pos %f, %f\n", q.x0, q.y1);

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
        ++msg;
        ++i;
    }

    glGenBuffers(1, &text->buffer);
    glBindBuffer(GL_ARRAY_BUFFER, text->buffer);
    glBufferData(GL_ARRAY_BUFFER, msg_len * quad_size, (const GLvoid *)quads,
                 GL_STATIC_DRAW);
    free(quads);

    glGenVertexArrays(1, &text->vao);
    glBindVertexArray(text->vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4,
                          (const void *)(sizeof(GLfloat) * 2));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    text->count = 6 * msg_len;

    // load vertex shader
    GLuint vertex_shader = SHADER(text, vert, NULL);

    // load fragment shader
    GLuint fragment_shader = SHADER(text, frag, NULL);

    // create basic program
    text->program = link_program(2, (GLuint[]){vertex_shader, fragment_shader});
}

void text_draw(text_t *text, mat4 view, mat4 projection) {
    mat4 model;
    float scale = 1. / 32;
    glm_scale_make(model, (vec3){scale, scale, scale});

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text->ftex);

    glUseProgram(text->program);
    glUniform1i(glGetUniformLocation(text->program, VAR_u_FontSampler), 0);
    glUniformMatrix4fv(glGetUniformLocation(text->program, VAR_u_Model), 1,
                       GL_FALSE, (float *)model);
    glUniformMatrix4fv(glGetUniformLocation(text->program, VAR_u_View), 1,
                       GL_FALSE, (float *)view);
    glUniformMatrix4fv(glGetUniformLocation(text->program, VAR_u_Projection), 1,
                       GL_FALSE, (float *)projection);

    glBindVertexArray(text->vao);
    glDrawArrays(GL_TRIANGLES, 0, text->count);
    glBindVertexArray(0);
}
