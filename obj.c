#include "obj.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "vec.h"

#define LINE_LEN 81

// number of GLfloats in vectors
#define VERTEX_COORD_LEN 3
#define TEXTURE_COORD_LEN 2
#define NORMAL_LEN 3

#define VERTS_IN_FACE 3

void parse_vector(vec_t *vec, char *str, size_t n) {
    str += 2; // skip "v*" from beginning of line
    for (size_t i=0; i<n; i++) {
        GLfloat f = strtof(str, &str);
        vec_push(vec, (void*)&f);
    }
}

size_t parse_face(vec_t *vec, char *str, size_t n) {
    char *token = strtok(str, " /"); // init and skip first token which is "f"
    while ((token = strtok(NULL, " /"))) {
        if (!n) {
            fprintf(stderr, "Warning: face may be parsed incorrectly.\n"
                    "Only triangles are supported.\n");
            break; // limit to n iterations
        }
        if (!token[0]) continue; // skip empty
        size_t i = strtoul(token, NULL, 0);
        if (i == 0) break; // check for error
        vec_push(vec, (void*)&i);
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

    // Set features to none so far
    *features = 0;

    // Allocate temp vecs for holding mesh data
    vec_t *vertices = vec_init(sizeof(GLfloat));
    if (!vertices) goto cleanup_file;
    vec_t *texture_coordinates = vec_init(sizeof(GLfloat));
    if (!texture_coordinates) goto cleanup_vertices;
    vec_t *normals = vec_init(sizeof(GLfloat));
    if (!normals) goto cleanup_texture_coordinates;
    vec_t *indices = vec_init(sizeof(size_t));
    if (!indices) goto cleanup_normals;

    // variables for deindexing
    vec_t *vector_vecs[] = {vertices, NULL, NULL};
    size_t vector_lens[] = {VERTEX_COORD_LEN, 0, 0};
    size_t vec_at = 1;

    // read file to vectors
    char line[LINE_LEN];
    size_t line_number = 0, faces = 0;
    size_t vertex_len = VERTEX_COORD_LEN, indices_in_vertex = 1;
    while (fgets(line, LINE_LEN, file)) {
        line_number++;
        if (line[0] == '#' || line[0] == '\n') continue; // ignore comments
        if (line[0] == 'v') {
            if (line[1] == ' ') { // add a vertex coordinate
                parse_vector(vertices, line, VERTEX_COORD_LEN);
            } else if (line[1] == 't') { // add a texture coordinate
                parse_vector(texture_coordinates, line, TEXTURE_COORD_LEN);
            } else if (line[1] == 'n') { // add a normal
                parse_vector(normals, line, NORMAL_LEN);
            } else goto unsupported_line;
        } else if (line [0] == 'f' && line[1] == ' ') { // add face / indices
            faces++;
            if (!indices->use) { // first face determines the format
                size_t separators = charcount(line, '/');
                if (separators) {
                    if (isdigit(strchr(line, '/')[1])) { // has texture coords
                        indices_in_vertex++;
                        vector_vecs[vec_at] = texture_coordinates;
                        vertex_len += vector_lens[vec_at++] = TEXTURE_COORD_LEN;
                        *features |= OBJ_TEXTURE_COORDINATES;
                    }
                    if (separators == 6) { // has normals
                        indices_in_vertex++;
                        vector_vecs[vec_at] = normals;
                        vertex_len += vector_lens[vec_at++] = NORMAL_LEN;
                        *features |= OBJ_NORMALS;
                    } else if (separators != 3) goto unsupported_line;
                }
            }
            if (parse_face(indices, line,
                        indices_in_vertex * VERTS_IN_FACE)) {
                fprintf(stderr, "Index missing at line %lu\n", line_number);
                goto cleanup_indices;
            }
        } else goto unsupported_line;
    }

    // allocate *mesh
    size_t mesh_len = vertex_len * VERTS_IN_FACE * faces;
    *mesh = malloc(sizeof(GLfloat) * mesh_len);
    if (!*mesh) goto cleanup_indices;

    // deindex to *mesh
    size_t mesh_pos = 0;
    for (size_t face = 0; face < faces; face++) {
        for (size_t vertex = 0; vertex < VERTS_IN_FACE; vertex++) {
            for (size_t i = 0; i < indices_in_vertex; i++) {
                size_t index_of_face = indices_in_vertex * VERTS_IN_FACE * face;
                size_t index_of_vertex = indices_in_vertex * vertex;
                size_t index_at = index_of_face + index_of_vertex + i;
                size_t index = VEC_GET(indices, size_t, index_at) - 1;
                size_t float_index = vector_lens[i] * index;
                memcpy((*mesh) + mesh_pos,
                        &VEC_GET(vector_vecs[i], GLfloat, float_index),
                        sizeof(GLfloat) * vector_lens[i]);
                mesh_pos += vector_lens[i];
            }
        }
    }

    vec_free(indices);
    vec_free(normals);
    vec_free(texture_coordinates);
    vec_free(vertices);
    fclose(file);
    return VERTS_IN_FACE * faces;

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
