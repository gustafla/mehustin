out vec4 FragColor;

varying vec2 v_TexCoord;

uniform sampler2D u_InputSampler;
uniform vec2 u_Resolution;


vec3 sample(vec2 uv) {
    vec3 c = texture2D(u_InputSampler, uv).rgb;
#ifdef FIRST_PASS
    float brightness = dot(c, vec3(0.2126, 0.7152, 0.0722));
    if (brightness < 1.) {
        return vec3(0);
    }
#endif
    return c;
}

const float kernel[30] = float[30](0.026596, 0.026537, 0.026361, 0.026070, 0.025667, 0.025159, 0.024551, 0.023852, 0.023070, 0.022215, 0.021297, 0.020326, 0.019313, 0.018269, 0.017205, 0.016131, 0.015058, 0.013993, 0.012946, 0.011924, 0.010934, 0.009982, 0.009072, 0.008209, 0.007395, 0.006632, 0.005921, 0.005263, 0.004658, 0.004104);

void main() {
    vec2 pixel = 1. / u_Resolution;
    vec3 color = sample(v_TexCoord) * kernel[0];
    for (int i = 1; i < 30; i++) {
        vec2 offset = vec2(0);
#ifdef FIRST_PASS
        offset.x = pixel.x * float(i);
#else
        offset.y = pixel.y * float(i);
#endif
        color += sample(v_TexCoord + offset) * kernel[i];
        color += sample(v_TexCoord - offset) * kernel[i];
    }
    FragColor = vec4(color, 1.);
}
