out vec4 FragColor;

#ifndef TRESHOLD
#define TRESHOLD 1.
#endif

uniform sampler2D u_InputSampler;

void main() {
    vec3 c = texelFetch(u_InputSampler, ivec2(gl_FragCoord.xy), 0).rgb;
    float brightness = dot(c, vec3(0.2126, 0.7152, 0.0722));
    if (brightness < TRESHOLD) {
        discard;
    }
    FragColor = vec4(c, 1.);
}
