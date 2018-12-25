#include <stdio.h>
#include <stdlib.h>
#include "src/base/demo.h"

static const demo_t* demo;

int scene_init(const demo_t *a_demo) {
    demo = a_demo;
    return EXIT_SUCCESS;
}

void scene_free(void) {
}

void scene_render(void) {
}
