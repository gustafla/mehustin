layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in vec3 a_WorldPos;

varying vec3 v_Pos;
varying vec2 v_TexCoord;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    vec4 pos = u_View * vec4(a_WorldPos, 1.);
    pos = u_Projection * vec4(a_Pos * 0.01 + pos.xyz, 1.0);

    v_Pos = pos.xyz;
    v_TexCoord = a_TexCoord;

    gl_Position = pos;
}
