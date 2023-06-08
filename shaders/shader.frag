out vec4 FragColor;

uniform float u_Focus;
varying vec3 v_Pos;
varying vec2 v_TexCoord;
varying float v_FocusEffect;

void main() {
    vec3 light = vec3(0.4, 1.2, 2.5);
    if (length(v_TexCoord) > 1.) {
        discard;
    }
    float area = 3.14 * v_FocusEffect * v_FocusEffect;
    FragColor = vec4(light * area, area);
}
