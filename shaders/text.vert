layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in float a_Depth;

varying vec3 v_Pos;
varying vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    vec4 pos = u_View * u_Model * vec4(a_Pos, a_Depth, 1.);

    v_Pos = pos.xyz;
    v_TexCoord = a_TexCoord;
    
    gl_Position = u_Projection * pos;
}
