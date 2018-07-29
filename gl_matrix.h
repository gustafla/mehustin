#ifndef GL_MATRIX_H
#define GL_MATRIX_H

#include <GL/gl.h>

// Allocate a zero matrix
void matrix_init(GLfloat**);

// Translate a matrix by a vector
//
// - translation vector (vec3)
// - matrix
void matrix_translate(const GLfloat*, GLfloat*);

// Muliply matrices
//
// - first operand (output)
// - second operand
void matrix_multiply(GLfloat*, const GLfloat*);

// Generate an identity matrix
void matrix_identity(GLfloat*);

// Generate a frustum matrix
//
// - left, right, bottom and top bounds
// - near and far bounds
// - matrix
void matrix_frustum(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
        GLfloat*);

// Generate a perspective projection matrix
//
// - vertical fov
// - aspect ratio
// - near and far bounds
// - matrix
void matrix_perspective(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat*);

// Generate a translation matrix
//
// - translation vector (vec3)
// - matrix
void matrix_translation(const GLfloat*, GLfloat*);

// Generate a matrix from a rotation quaternion and a translation vector
//
// - rotation quaternion
// - translation vector (vec3)
// - matrix
void matrix_rotation_translation(const GLfloat*, const GLfloat*, GLfloat*);

// Generate a quaternion from euler angles
//
// - roll, pitch, yaw angles
// - quaternion
void quat_euler(GLfloat, GLfloat, GLfloat, GLfloat*);

// Generate a matrix from euler angles and a translation vector
//
// - roll, pitch, yaw angles
// - translation vector (vec3)
// - matrix
void matrix_euler_rotation_translation(GLfloat, GLfloat, GLfloat,
        const GLfloat*, GLfloat*);

#endif
