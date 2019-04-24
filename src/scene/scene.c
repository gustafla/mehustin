#include "scene.h"
#include <stdlib.h>
#include <stdio.h>

static double (*get_value)(const char *name);

int32_t scene_init(int32_t width, int32_t height, double (*getval)(const char*)) {
	get_value = getval;
	return EXIT_SUCCESS;
}

void scene_deinit(void) {
}

void scene_render(double time) {
	printf("Value of test is: %f\n", get_value("test"));
}
