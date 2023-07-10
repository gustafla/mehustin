out vec4 FragColor;

varying vec3 v_Pos;
varying vec2 v_TexCoord;

uniform int u_Layers;
uniform sampler2D u_FontSampler;

void main() {
    FragColor = vec4(texture2D(u_FontSampler, v_TexCoord).rrr * 1. / float(u_Layers), 1.0);
    //FragColor = vec4(1.0);
}
