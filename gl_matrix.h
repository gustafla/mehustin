#ifndef GL_MATRIX_H
#define GL_MATRIX_H

#include <GL/gl.h>

void matrix_frustum(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat*);
void matrix_perspective(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat*);

#endif
