#include "mesh.h"
#include <stdlib.h>
#include "obj.h"
#include "gl_util.h"

mesh_t *mesh_init_from_obj_file(const char *obj_file_path) {
	mesh_t *mesh = calloc(1, sizeof(mesh_t));
	if (!mesh) return NULL;

	GLfloat *data;
	obj_features_t features;
	size_t v_size;
	mesh->count = load_obj_deindexed(obj_file_path, &data, &features, &v_size);
	if (!mesh->count) {
		free(mesh);
		return NULL;
	}

	// buffer
	glGenBuffers(1, &mesh->array_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->array_buffer);
	glBufferData(GL_ARRAY_BUFFER, v_size * mesh->count, data, GL_STATIC_DRAW);

	// vao
	size_t i=1, offset = 3;
	const vertex_attrib_pointer_t *params[] = {
		&VAP(0, 3, v_size, 0), NULL, NULL, NULL
	};
	if (features & OBJ_TEXTURE_COORDINATES) {
		params[i] = &VAP(1, 2, v_size, offset * sizeof(GLfloat));
		offset += 2;
		i++;
	}
	if (features & OBJ_NORMALS) {
		params[i] = &VAP(2, 3, v_size, offset * sizeof(GLfloat));
	}
	mesh->vao = gen_vao(params);

	return mesh;
}

void mesh_free(mesh_t *mesh) {
	if (mesh) {
		glDeleteBuffers(1, &mesh->array_buffer);
		glDeleteVertexArrays(1, &mesh->vao);
		free(mesh);
	}
}

void mesh_draw(mesh_t *mesh) {
	glBindVertexArray(mesh->vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh->count);
	glBindVertexArray(0);
}
