#ifndef VEC_H
#define VEC_H

#include <stddef.h>

#define VEC_GET(vec, type, index) (((type*)vec->buffer)[index])

typedef struct {
	char *buffer;
	size_t element_size;
	size_t use;
	size_t capacity;
} vec_t;

vec_t *vec_init(size_t element_size);
void vec_free(vec_t *vec);
int vec_append(vec_t *vec, const void *array, size_t elements);
int vec_push(vec_t *vec, const void *value);

#endif
