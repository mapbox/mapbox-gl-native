attribute vec2 a_pos;
attribute vec2 a_tex;
attribute float a_minzoom;

uniform mat4 u_matrix;
uniform float u_size;
uniform float u_ratio;
uniform float u_zoom;

varying vec2 v_tex;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
    gl_PointSize = u_size;
    v_tex = a_tex * u_ratio;

    gl_Position.z = 1.0 - step(a_minzoom, u_zoom);
}
