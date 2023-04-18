#version 330 core

out vec4 FragColor;

varying vec3 v_Pos;
varying vec2 v_TexCoord;

uniform sampler2D u_InputSampler;
uniform sampler2D u_PerlinSampler;
uniform sampler2D u_RandSampler;
uniform float u_Brightness;
uniform vec2 u_NoiseScale;

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

vec3 rgb_pattern(vec2 uv, float freq) {
    vec3 pattern = vec3(
        sin(uv.x * PI * 2. * freq),
        sin(((uv.x * PI * 2.) + (PI * 2)/3) * freq),
        sin(((uv.x * PI * 2.) + (PI * 4)/3) * freq));
    pattern = clamp(pattern, 0. ,1.);
    pattern *= clamp(sin(uv.y * PI * 4. * freq + floor(uv.x * PI * freq) * PI) * 2. + 2.3, 0., 1.);
    return clamp(pattern, 0., 1.);
}

void main() {
    // coordinates
    vec2 center = v_TexCoord - vec2(0.5);
    vec2 distor = center * vec2(
            1. + abs(center.y * center.y) * 0.1,
            1. + abs(center.x * center.x) * 0.2) + vec2(0.5);

    if (distor.x < 0. || distor.x > 1. || distor.y < 0. || distor.y > 1.) {
        discard; // Hide "overscan" at edges
    }

    // Input color
    vec3 color = texture2D(u_InputSampler, distor).rgb;

    // Add perlin and random rgb noises
    vec2 noise_coord = v_TexCoord * u_NoiseScale;
    float noise_perlin = texture2D(u_PerlinSampler, noise_coord).r;
    vec3 noise_rand = texture2D(u_RandSampler, noise_coord).rgb;
    vec3 noise = noise_perlin * 0.02 + noise_rand * 0.06;
    noise -= 0.04;
    color += noise;

    // Vignette
    color -= length(center * 0.12);

    // Add texture to screen edges
    color *= edge_pattern(distor, 300., 3.5);

    // Add rgb pattern
    color *= rgb_pattern(distor, 600.) * 0.2 + 0.8;

    FragColor = vec4(color + vec3(u_Brightness), 1.);
}
