#include "obj.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINE_LEN 81

typedef struct {
    void *buffer;
    size_t element_size;
    size_t use;
    size_t capacity;
} vec_t;

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

int vec_add(vec_t *vec, const void *value) {
    if (!vec || !vec->buffer) return EXIT_FAILURE;
    if (vec->use == vec->capacity) {
        vec->capacity *= 2;
        vec->buffer = realloc(vec->buffer, vec->element_size * vec->capacity);
        if (!vec->buffer) return EXIT_FAILURE;
    }
    size_t offset = vec->use * vec->element_size;
    memcpy(((unsigned char*)vec->buffer) + offset, value, vec->element_size);
    return EXIT_SUCCESS;
}

void parse_vertex(vec_t *vec, char *str, size_t n) {
    for (size_t i=0; i<n; i++) {
        GLfloat f = strtof(str, &str);
        vec_add(vec, (void*)&f);
    }
}

size_t parse_face(vec_t *vec, char *str, size_t n) {
    char *token = strtok(str, " /"); // init and skip first token which is "f"
    while ((token = strtok(NULL, " /"))) {
        if (!n) break; // limit to n iterations
        if (!token[0]) continue; // skip empty
        GLuint i = strtoul(token, NULL, 0);
        if (i == 0) break; // check for error
        vec_add(vec, (void*)&i);
        n--;
    }
    return n;
}

size_t charcount(const char *str, char c) {
    size_t count = 0;
    while (*(str++)) {
        if (*str == c) count++;
    }
    return count;
}

size_t load_obj_deindexed(const char *obj_file_path, GLfloat **mesh,
       obj_features_t *features) {
    FILE *file = fopen(obj_file_path, "r");
    if (!file) goto error;

    *features = 0;

    vec_t *vertices = vec_init(sizeof(GLfloat));
    if (!vertices) goto cleanup_file;
    vec_t *texture_coordinates = vec_init(sizeof(GLfloat));
    if (!texture_coordinates) goto cleanup_vertices;
    vec_t *normals = vec_init(sizeof(GLfloat));
    if (!normals) goto cleanup_texture_coordinates;
    vec_t *indices = vec_init(sizeof(GLuint));
    if (!indices) goto cleanup_normals;

    // read file to vectors
    char line[LINE_LEN];
    size_t line_number = 0, vertex_len = 3, face_len = 1, faces = 0;
    while (fgets(line, LINE_LEN, file)) {
        line_number++;
        if (line[0] == '#' || line[0] == '\n') continue;
        if (line[0] == 'v') {
            if (line[1] == ' ') { // Add a vertex coordinate
                if (indices->use) goto unsupported_line;
                parse_vertex(vertices, line + 1, 3);
            } else if (line[1] == 't') { // Add a texture coordinate
                if (indices->use) goto unsupported_line;
                parse_vertex(texture_coordinates, line + 2, 2);
            } else if (line[1] == 'n') { // Add a normal
                if (indices->use) goto unsupported_line;
                parse_vertex(normals, line + 2, 3);
            } else goto unsupported_line;
        } else if (line [0] == 'f' && line[1] == ' ') { // Add indices
            faces++;
            if (!indices->use) { // First index determines the format
                switch (charcount(line, '/')) {
                    case 6:
                        // If first slash doesn't have another next to it
                        if (*(strchr(line, '/') + 1) != '/') {
                            vertex_len += 2; // has texture coordinates
                            *features |= OBJ_TEXTURE_COORDINATES;
                        }
                        vertex_len += 3; // has normals
                        face_len += 2;
                        *features |= OBJ_NORMALS;
                        break;
                    case 3:
                        vertex_len += 2; // has texture coordinates
                        face_len += 1;
                        *features |= OBJ_TEXTURE_COORDINATES;
                        break;
                    case 0: break;
                    default: goto unsupported_line;
                }
            }
            if (parse_face(indices, line, face_len * 3)) {
                fprintf(stderr, "Index missing at line %lu\n", line_number);
                goto cleanup_indices;
            }
        } else goto unsupported_line;
    }

    // allocate *mesh
    size_t mesh_len = vertex_len * 3 * faces;
    *mesh = malloc(sizeof(GLfloat) * mesh_len);
    if (!*mesh) goto cleanup_indices;

    // TODO fucking rewrite this as soon as you dare
    // deindex vectors to *mesh
    GLuint *i = (GLuint*)indices->buffer;
    for (size_t mesh_pos = 0, face = 0; mesh_pos < mesh_len; face++) {
        for (size_t vertex = 0, index; vertex < 3; vertex++) { // vertex of face
            // vertex coordinate
            GLuint offset = i[face * face_len * 3 + vertex * face_len] - 1;
            memcpy((*mesh) + mesh_pos, ((GLfloat*)vertices->buffer)
                    + 3 * offset, sizeof(GLfloat) * 3);
            mesh_pos += 3;

            // texture coordinate
            index = 1;
            if (*features & OBJ_TEXTURE_COORDINATES) {
                GLuint offset = i[face * face_len * 3 + vertex * face_len
                    + index] - 1;
                memcpy((*mesh) + mesh_pos,
                        ((GLfloat*)texture_coordinates->buffer) + 2 * offset,
                        sizeof(GLfloat) * 2);
                mesh_pos += 2;
                index++;
            }

            // normal
            if (*features & OBJ_NORMALS) {
                GLuint offset = i[face * face_len * 3 + vertex * face_len
                    + index] - 1;
                memcpy((*mesh) + mesh_pos, ((GLfloat*)normals->buffer)
                        + 3 * offset, sizeof(GLfloat) * 3);
                mesh_pos += 3;
            }
        }
    }

    vec_free(indices);
    vec_free(normals);
    vec_free(texture_coordinates);
    vec_free(vertices);
    fclose(file);
    return 3 * faces;

unsupported_line:
    fprintf(stderr, "Unsupported OBJ line %lu\n", line_number);
cleanup_indices:
    vec_free(indices);
cleanup_normals:
    vec_free(normals);
cleanup_texture_coordinates:
    vec_free(texture_coordinates);
cleanup_vertices:
    vec_free(vertices);
cleanup_file:
    fclose(file);
error:
    fprintf(stderr, "Failed to load obj file: %s\n", obj_file_path);
    return 0;
}
