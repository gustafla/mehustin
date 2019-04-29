#ifndef OBJ_H
#define OBJ_H

#include <stddef.h>
#include <GLES2/gl2.h>

typedef enum {
    OBJ_NORMALS = 0x1,
    OBJ_TEXTURE_COORDINATES = 0x2
} obj_features_t;

size_t load_obj_deindexed(const char*, GLfloat**, obj_features_t*, size_t*);

#endif
