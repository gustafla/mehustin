#include "gl_matrix.h"
#include <math.h>

void matrix_frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
        GLfloat near, GLfloat far, GLfloat *matrix) {
    GLfloat rl = (right - left), tb = (top - bottom), fn = (far - near);
    matrix[0] = (near * 2) / rl;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;
    matrix[4] = 0;
    matrix[5] = (near * 2) / tb;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = (right + left) / rl;
    matrix[9] = (top + bottom) / tb;
    matrix[10] = -(far + near) / fn;
    matrix[11] = -1;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = -(far * near * 2) / fn;
    matrix[15] = 0;
}

void matrix_perspective(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far,
        GLfloat *matrix) {
    GLfloat top = near * tan(fovy * 3.14159265358979323846 / 360.0);
    GLfloat right = top * aspect;
    matrix_frustum(-right, right, -top, top, near, far, matrix);
}
