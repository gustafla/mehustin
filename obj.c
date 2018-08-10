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

#define VERTICES_PER_FACE 3

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
        GLuint i = strtoul(token, NULL, 0);
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
    vec_t *indices = vec_init(sizeof(GLuint));
    if (!indices) goto cleanup_normals;

    // read file to vectors
    char line[LINE_LEN];
    size_t line_number = 0, vertex_len = 3, indices_per_face = 1, faces = 0;
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
                        indices_per_face++;
                        vertex_len += TEXTURE_COORD_LEN;
                        *features |= OBJ_TEXTURE_COORDINATES;
                    }
                    if (separators == 6) { // has normals
                        indices_per_face++;
                        vertex_len += NORMAL_LEN;
                        *features |= OBJ_NORMALS;
                    } else if (separators != 3) goto unsupported_line;
                }
            }
            if (parse_face(indices, line,
                        indices_per_face * VERTICES_PER_FACE)) {
                fprintf(stderr, "Index missing at line %lu\n", line_number);
                goto cleanup_indices;
            }
        } else goto unsupported_line;
    }

    // allocate *mesh
    size_t mesh_len = vertex_len * VERTICES_PER_FACE * faces;
    *mesh = malloc(sizeof(GLfloat) * mesh_len);
    if (!*mesh) goto cleanup_indices;

    // TODO fucking rewrite this as soon as you dare
    // deindex vectors to *mesh
    GLuint *i = (GLuint*)indices->buffer;
    for (size_t mesh_pos = 0, face = 0; mesh_pos < mesh_len; face++) {
        for (size_t vertex = 0, index; vertex < 3; vertex++) { // vertex of face
            // vertex coordinate
            GLuint offset = i[face * indices_per_face * 3 + vertex * indices_per_face] - 1;
            memcpy((*mesh) + mesh_pos, ((GLfloat*)vertices->buffer)
                    + 3 * offset, sizeof(GLfloat) * 3);
            mesh_pos += 3;

            // texture coordinate
            index = 1;
            if (*features & OBJ_TEXTURE_COORDINATES) {
                GLuint offset = i[face * indices_per_face * 3 + vertex * indices_per_face
                    + index] - 1;
                memcpy((*mesh) + mesh_pos,
                        ((GLfloat*)texture_coordinates->buffer) + 2 * offset,
                        sizeof(GLfloat) * 2);
                mesh_pos += 2;
                index++;
            }

            // normal
            if (*features & OBJ_NORMALS) {
                GLuint offset = i[face * indices_per_face * 3 + vertex * indices_per_face
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
    return VERTICES_PER_FACE * faces;

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
