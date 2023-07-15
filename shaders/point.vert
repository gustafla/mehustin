layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in vec3 a_WorldPos;

varying vec3 v_Pos;
varying vec2 v_TexCoord;
varying float v_FocusEffect;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform float u_Focus;

void main() {
    vec4 pos = u_View * vec4(a_WorldPos, 1.);
    float focus_effect = clamp(abs(pos.z - u_Focus), 0.2, 1.);
    pos = vec4(a_Pos * max(0.01 * focus_effect, 0.006) + pos.xyz, 1.0);

    v_Pos = pos.xyz;
    v_TexCoord = a_TexCoord * 2. - 1.;
    v_FocusEffect = 1. - focus_effect;
    
    gl_Position = u_Projection * pos;
}
