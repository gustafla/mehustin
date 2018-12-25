#ifndef OBJ_H
#define OBJ_H

#include "GL/gl.h"

typedef enum {
    OBJ_NORMALS = 0x1,
    OBJ_TEXTURE_COORDINATES = 0x2
} obj_features_t;

size_t load_obj_deindexed(const char*, GLfloat**, obj_features_t*, size_t*);

#endif
