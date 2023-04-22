#version 330 core

out vec4 FragColor;

varying vec3 v_Pos;
varying vec2 v_TexCoord;

uniform sampler2D u_InputSampler;
uniform sampler2D u_BloomSampler;
uniform sampler2D u_PerlinSampler;
uniform sampler2D u_RandSampler;
uniform float u_Brightness;
uniform float u_NoiseSize;
uniform vec2 u_Resolution;

#define PI 3.14159265

float pattern(vec2 uv, float freq) {
    float f = freq * PI * 2.;
    return sin(uv.x * f) * sin(uv.y * f);
}

float edge_pattern(vec2 uv, float freq, float grad) {
    float pattern = pattern(uv, freq) + 2.;
    pattern *= sin(uv.x * PI) * grad;
    pattern *= sin(uv.y * PI) * grad;
    return clamp(pattern, 0. ,1.);
}

vec3 rgb_pattern(vec2 uv, vec2 freq) {
    vec3 pattern = vec3(
        sin(uv.x * PI * 2. * freq.x),
        sin(uv.x * PI * 2. * freq.x + (PI * 2. / 3)),
        sin(uv.x * PI * 2. * freq.x + (PI * 2. / 3) * 2));
    return clamp(pattern, 0., 1.);
}

void main() {
    // coordinates
    vec2 center = v_TexCoord - vec2(0.5);
    vec2 distor = center * vec2(
            1. + abs(center.y * center.y) * 0.1,
            1. + abs(center.x * center.x) * 0.2) + vec2(0.5);

    // Input color
    vec3 color = texture2D(u_InputSampler, distor).rgb;

    // Add perlin and random rgb noises
    vec2 noise_scale = u_Resolution / u_NoiseSize;
    vec2 noise_coord = v_TexCoord * noise_scale;
    float noise_perlin = texture2D(u_PerlinSampler, noise_coord).r;
    vec3 noise_rand = texture2D(u_RandSampler, noise_coord).rgb;
    vec3 noise = noise_perlin * 0.02 + noise_rand * 0.06;
    noise -= 0.04;
    color += noise;

    // Vignette
    color -= length(center * 0.12);

    // Add texture to screen edges
    color *= edge_pattern(distor, 300., 3.5);

    // Add rgb pattern and scanlines
    vec2 freq = vec2(u_Resolution.x / 3.2, u_Resolution.y / 2.4);
    color *= rgb_pattern(distor, freq) * 0.2 + 0.8;
    color *= mod(gl_FragCoord.y, 2.) * 0.1 + 0.9;

    vec3 bloom = texture2D(u_BloomSampler, distor).rgb;
    FragColor = vec4(color + vec3(u_Brightness) + bloom, 1.);
}
