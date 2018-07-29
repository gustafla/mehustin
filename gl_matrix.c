#include "gl_matrix.h"
#include <math.h>
#include <stdlib.h>

void matrix_init(GLfloat **matrix) {
    *matrix = (GLfloat*)calloc(16, sizeof(GLfloat));
}

void matrix_translate(const GLfloat *vec, GLfloat *matrix) {
    GLfloat x = vec[0], y = vec[1], z = vec[2];

    matrix[12] = matrix[0] * x + matrix[4] * y + matrix[8] * z + matrix[12];
    matrix[13] = matrix[1] * x + matrix[5] * y + matrix[9] * z + matrix[13];
    matrix[14] = matrix[2] * x + matrix[6] * y + matrix[10] * z + matrix[14];
    matrix[15] = matrix[3] * x + matrix[7] * y + matrix[11] * z + matrix[15];
}

void matrix_multiply(GLfloat *mat, const GLfloat *mat2) {
    GLfloat a00 = mat[0], a01 = mat[1], a02 = mat[2], a03 = mat[3];
    GLfloat a10 = mat[4], a11 = mat[5], a12 = mat[6], a13 = mat[7];
    GLfloat a20 = mat[8], a21 = mat[9], a22 = mat[10], a23 = mat[11];
    GLfloat a30 = mat[12], a31 = mat[13], a32 = mat[14], a33 = mat[15];

    GLfloat b00 = mat2[0], b01 = mat2[1], b02 = mat2[2], b03 = mat2[3];
    GLfloat b10 = mat2[4], b11 = mat2[5], b12 = mat2[6], b13 = mat2[7];
    GLfloat b20 = mat2[8], b21 = mat2[9], b22 = mat2[10], b23 = mat2[11];
    GLfloat b30 = mat2[12], b31 = mat2[13], b32 = mat2[14], b33 = mat2[15];

    mat[0] = b00 * a00 + b01 * a10 + b02 * a20 + b03 * a30;
    mat[1] = b00 * a01 + b01 * a11 + b02 * a21 + b03 * a31;
    mat[2] = b00 * a02 + b01 * a12 + b02 * a22 + b03 * a32;
    mat[3] = b00 * a03 + b01 * a13 + b02 * a23 + b03 * a33;
    mat[4] = b10 * a00 + b11 * a10 + b12 * a20 + b13 * a30;
    mat[5] = b10 * a01 + b11 * a11 + b12 * a21 + b13 * a31;
    mat[6] = b10 * a02 + b11 * a12 + b12 * a22 + b13 * a32;
    mat[7] = b10 * a03 + b11 * a13 + b12 * a23 + b13 * a33;
    mat[8] = b20 * a00 + b21 * a10 + b22 * a20 + b23 * a30;
    mat[9] = b20 * a01 + b21 * a11 + b22 * a21 + b23 * a31;
    mat[10] = b20 * a02 + b21 * a12 + b22 * a22 + b23 * a32;
    mat[11] = b20 * a03 + b21 * a13 + b22 * a23 + b23 * a33;
    mat[12] = b30 * a00 + b31 * a10 + b32 * a20 + b33 * a30;
    mat[13] = b30 * a01 + b31 * a11 + b32 * a21 + b33 * a31;
    mat[14] = b30 * a02 + b31 * a12 + b32 * a22 + b33 * a32;
    mat[15] = b30 * a03 + b31 * a13 + b32 * a23 + b33 * a33;
}

void matrix_identity(GLfloat *matrix) {
    matrix[0] = 1; matrix[1] = 0; matrix[2] = 0; matrix[3] = 0;
    matrix[4] = 0; matrix[5] = 1; matrix[6] = 0; matrix[7] = 0;
    matrix[8] = 0; matrix[9] = 0; matrix[10] = 1; matrix[11] = 0;
    matrix[12] = 0; matrix[13] = 0; matrix[14] = 0; matrix[15] = 1;
}

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


void matrix_translation(const GLfloat *vec, GLfloat *matrix) {
    GLfloat x = vec[0], y = vec[1], z = vec[2];

    matrix[0] = 1; matrix[1] = 0; matrix[2] = 0; matrix[3] = 0;
    matrix[4] = 0; matrix[5] = 1; matrix[6] = 0; matrix[7] = 0;
    matrix[8] = 0; matrix[9] = 0; matrix[10] = 1; matrix[11] = 0;
    matrix[12] = x; matrix[13] = y; matrix[14] = z; matrix[15] = 1;
}

void matrix_rotation_translation(const GLfloat *quat, const GLfloat *vec,
        GLfloat *matrix) {
    // quaternion math
    GLfloat x = quat[0], y = quat[1], z = quat[2], w = quat[3];
    GLfloat x2 = x + x, y2 = y + y, z2 = z + z;
    GLfloat xx = x * x2, xy = x * y2, xz = x * z2,
            yy = y * y2, yz = y * z2, zz = z * z2,
            wx = w * x2, wy = w * y2, wz = w * z2;

    matrix[0] = 1 - (yy + zz);
    matrix[1] = xy + wz;
    matrix[2] = xz - wy;
    matrix[3] = 0;
    matrix[4] = xy - wz;
    matrix[5] = 1 - (xx + zz);
    matrix[6] = yz + wx;
    matrix[7] = 0;
    matrix[8] = xz + wy;
    matrix[9] = yz - wx;
    matrix[10] = 1 - (xx + yy);
    matrix[11] = 0;
    matrix[12] = vec[0];
    matrix[13] = vec[1];
    matrix[14] = vec[2];
    matrix[15] = 1;
}

void quat_euler(GLfloat roll, GLfloat pitch, GLfloat yaw, GLfloat *quat) {
    // calculate trig identities
    GLfloat cr = cos(roll/2), cp = cos(pitch/2), cy = cos(yaw/2),
            sr = sin(roll/2), sp = sin(pitch/2), sy = sin(yaw/2),
            cpcy = cp * cy, spsy = sp * sy;
    quat[0] = sr * cpcy - cr * spsy;
    quat[1] = cr * sp * cy + sr * cp * sy;
    quat[2] = cr * cp * sy - sr * sp * cy;
    quat[3] = cr * cpcy + sr * spsy;
}

void matrix_euler_rotation_translation(GLfloat roll, GLfloat pitch, GLfloat yaw,
        const GLfloat *vec, GLfloat *matrix) {
    GLfloat quat[4];
    quat_euler(roll, pitch, yaw, quat);
    matrix_rotation_translation(quat, vec, matrix);
}
