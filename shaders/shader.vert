#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec2 a_TexCoord;

varying vec3 v_Pos;
varying vec2 v_TexCoord;

void main() {
    v_Pos = a_Pos;
    v_TexCoord = a_TexCoord;
    gl_Position = vec4(a_Pos, 1.0);
}
