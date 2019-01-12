#ifndef MESH_H
#define MESH_H

#include <GLES2/gl2.h>

typedef struct {
	GLuint array_buffer;
	GLuint vao;
	size_t count;
} mesh_t;

mesh_t *mesh_init_from_obj_file(const char *obj_file_path);
void mesh_free(mesh_t *mesh);
void mesh_draw(mesh_t *mesh);

#endif
