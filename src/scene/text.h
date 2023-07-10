#ifndef TEXT_H
#define TEXT_H
#undef STB_TRUETYPE_IMPLEMENTATION
#include "cglm/mat4.h"
#include "gl.h"
#include "stb_truetype.h"

typedef struct text_t_ {
    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    GLuint ftex;
    GLuint vao;
    GLuint buffer;
    GLuint depth_instance_buffer;
    GLsizei count;
    GLsizei depth;
    GLuint program;
} text_t;

void text_init(text_t *text, char *msg);
void text_draw(text_t *text, mat4 view, mat4 projection);

#endif
