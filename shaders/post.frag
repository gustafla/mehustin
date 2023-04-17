#version 330 core

out vec4 FragColor;

varying vec3 v_Pos;
varying vec2 v_TexCoord;

uniform sampler2D u_InputSampler;
uniform sampler2D u_PerlinSampler;
uniform sampler2D u_RandSampler;
uniform float u_Brightness;

void main() {
    float noise_perlin = texture2D(u_PerlinSampler, v_TexCoord).r;
    vec3 noise_rand = texture2D(u_RandSampler, v_TexCoord).rgb;
    vec3 noise = noise_perlin * 0.02 + noise_rand * 0.06;
    noise -= 0.04;
    vec3 color = texture2D(u_InputSampler, v_TexCoord + vec2(sin(v_TexCoord.y * 10.), 0.)).rgb;
    FragColor = vec4(color + vec3(u_Brightness) + noise, 1.);
}
