#version 330 core

out vec4 FragColor;

varying vec3 v_Pos;
varying vec2 v_TexCoord;

uniform sampler2D u_InputSampler;
uniform sampler2D u_PerlinSampler;
uniform sampler2D u_RandSampler;
uniform float u_Brightness;
uniform vec2 u_NoiseScale;

void main() {
    // coordinates
    vec2 center = v_TexCoord - vec2(0.5);
    vec2 distor = center * vec2(
            1. + abs(center.y * center.y) * 0.1,
            1. + abs(center.x * center.x) * 0.2) + vec2(0.5);

    if (distor.x < 0. || distor.x > 1. || distor.y < 0. || distor.y > 1.) {
        discard; // Hide "overscan" at edges
    }

    vec2 noise_coord = v_TexCoord * u_NoiseScale;
    
    float noise_perlin = texture2D(u_PerlinSampler, noise_coord).r;
    vec3 noise_rand = texture2D(u_RandSampler, noise_coord).rgb;
    vec3 noise = noise_perlin * 0.02 + noise_rand * 0.06;
    noise -= 0.04;

    vec3 color = texture2D(u_InputSampler, distor).rgb;
    color -= length(center * 0.12); // vignette
    FragColor = vec4(color + vec3(u_Brightness) + noise, 1.);
}
