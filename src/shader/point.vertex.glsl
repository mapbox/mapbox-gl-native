attribute vec2 a_pos;

uniform mat4 u_matrix;
uniform float u_size;

void main() {
    float z = step(32767.0, a_pos.x) * 2.0;
    gl_Position = u_matrix * vec4(a_pos, z, 1);
    gl_PointSize = u_size;
}
