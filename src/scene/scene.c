#include "scene.h"
#include <stdlib.h>
#include <stdio.h>
#include <GLES2/gl2.h>

typedef struct scene_t_ {
    double (*get_value)(const char*);
} scene_t;

void* scene_init(int32_t width, int32_t height, double (*getval)(const char*)) {
    // set up viewport
    glViewport(0, 0, width, height);

    scene_t *scene = malloc(sizeof(scene_t));
    scene->get_value = getval;

    return scene;
}

void scene_deinit(void *data) {
    free(data);
}

void scene_render(double time, void *data) {
    scene_t *scene = data;

#ifdef DEBUG
    // check opengl errors
    switch (glGetError()) {
        case GL_NO_ERROR: break;
        case GL_INVALID_ENUM: fprintf(stderr, "GL_INVALID_ENUM\n"); break;
        case GL_INVALID_VALUE: fprintf(stderr, "GL_INVALID_VALUE\n"); break;
        case GL_INVALID_OPERATION: fprintf(stderr, "GL_INVALID_OPERATION\n"); break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: fprintf(stderr, "GL_INVALID_FRAMEBUFFER_OPERATION\n"); break;
        case GL_OUT_OF_MEMORY: fprintf(stderr, "GL_OUT_OF_MEMORY\n"); break;
        default: fprintf(stderr, "Unknown glGetError() return value\n");
    }
#endif

    printf("Value of test is: %f\n", scene->get_value("test"));
}
