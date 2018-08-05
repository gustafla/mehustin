#include <stdio.h>
#include "demo.h"

static demo_t* demo;

void scene_init(demo_t* p_demo) {
    demo = p_demo;
}

void scene_deinit() {
    
}

void scene_render() {
    printf("scene_render(). demo@%p\n", demo);
}
