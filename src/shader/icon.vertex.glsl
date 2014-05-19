attribute vec2 a_pos;
attribute vec2 a_tex;

uniform mat4 u_matrix;
uniform float u_size;
uniform float u_ratio;

varying vec2 v_tex;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
    gl_PointSize = u_size;
    v_tex = a_tex * u_ratio;
}
