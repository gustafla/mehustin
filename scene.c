#include <stdio.h>
#include <stdlib.h>
#include "demo.h"

static demo_t* demo;

int scene_init(const demo_t *p_demo) {
    demo = p_demo;
    return EXIT_SUCCESS;
}

void scene_deinit() {
    
}

void scene_render() {
    printf("scene_render(). demo@%p\n", demo);
}
