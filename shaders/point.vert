layout (location = 0) in vec3 a_Pos;

uniform mat4 u_Projection;

const vec2 screen_offsets[6] = vec2[6](
        vec2(-1, -1),
        vec2( 1, -1),
        vec2( 1,  1),
        vec2(-1, -1),
        vec2( 1,  1),
        vec2(-1,  1)
    );

void main() {
    vec4 pos = vec4(a_Pos, 1.0) * u_Projection;
    vec2 off = screen_offsets[gl_VertexID % 6] * 0.01;
    gl_Position = vec4(pos.xy + off, pos.zw);
}
