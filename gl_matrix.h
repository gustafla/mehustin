#ifndef GL_MATRIX_H
#define GL_MATRIX_H

#include <GL/gl.h>

// get the length of a vec3
GLfloat vec3_length(const GLfloat*);

// normalize a vec3
void vec3_normalize(GLfloat*);

// vec3 cross product
//
// - first operand (output)
// - second operand
void vec3_cross(GLfloat*, const GLfloat*);

// allocate a zero matrix
void matrix_init(GLfloat**);

// translate a matrix by a vector
//
// - translation vector (vec3)
// - matrix
void matrix_translate(const GLfloat*, GLfloat*);

// muliply matrices
//
// - first operand (output)
// - second operand
void matrix_multiply(GLfloat*, const GLfloat*);

// generate an identity matrix
void matrix_identity(GLfloat*);

// generate a frustum matrix
//
// - left, right, bottom and top bounds
// - near and far bounds
// - matrix
void matrix_frustum(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
        GLfloat*);

// generate a perspective projection matrix
//
// - vertical fov
// - aspect ratio
// - near and far bounds
// - matrix
void matrix_perspective(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat*);

// generate a translation matrix
//
// - translation vector (vec3)
// - matrix
void matrix_translation(const GLfloat*, GLfloat*);

// generate a matrix from a rotation quaternion and a translation vector
//
// - rotation quaternion
// - translation vector (vec3)
// - matrix
void matrix_rotation_translation(const GLfloat*, const GLfloat*, GLfloat*);

// generate a quaternion from euler angles
//
// - roll, pitch, yaw angles
// - quaternion
void quat_euler(GLfloat, GLfloat, GLfloat, GLfloat*);

// generate a matrix from euler angles and a translation vector
//
// - roll, pitch, yaw angles
// - translation vector (vec3)
// - matrix
void matrix_euler_rotation_translation(GLfloat, GLfloat, GLfloat,
        const GLfloat*, GLfloat*);

// generate a camera matrix from position and target point
//
// - camera position
// - target
// - matrix
void matrix_look_at(const GLfloat*, const GLfloat*, GLfloat*);

#endif
