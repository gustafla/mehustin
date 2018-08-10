#include "vec.h"
#include <stdlib.h>
#include <string.h>

vec_t *vec_init(size_t element_size) {
    vec_t *vec = calloc(1, sizeof(vec_t));
    if (!vec) return NULL;
    vec->capacity = 16;
    vec->element_size = element_size;
    vec->buffer = calloc(vec->capacity, vec->element_size);
    if (!vec->buffer) {
        free(vec);
        return NULL;
    }
    return vec;
}

void vec_free(vec_t *vec) {
    if (vec) {
        if (vec->buffer) free(vec->buffer);
        free(vec);
    }
}

int vec_append(vec_t *vec, const void *array, size_t elements) {
    if (!vec || !vec->buffer || !array) return EXIT_FAILURE;

    int realloc_needed = 0;
    while (vec->capacity < vec->use + elements) {
        vec->capacity = (vec->capacity * 3) / 2 + 1;
        realloc_needed = 1;
    }

    if (realloc_needed) {
        vec->buffer = realloc(vec->buffer, vec->element_size * vec->capacity);
        if (!vec->buffer) return EXIT_FAILURE;
    }

    size_t offset = vec->use * vec->element_size;
    memcpy(vec->buffer + offset, array, vec->element_size * elements);
    vec->use += elements;

    return EXIT_SUCCESS;
}

int vec_push(vec_t *vec, const void *value) {
    return vec_append(vec, value, 1);
}
