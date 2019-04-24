#include "scene.h"
#include <stdlib.h>
#include <stdio.h>
#include <GLES2/gl2.h>

static double (*get_value)(const char *name);

int32_t scene_init(int32_t width, int32_t height, double (*getval)(const char*)) {
	get_value = getval;

	// set up viewport
	glViewport(0, 0, width, height);

	return EXIT_SUCCESS;
}

void scene_deinit(void) {
}

void scene_render(double time) {
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

	printf("Value of test is: %f\n", get_value("test"));
}
