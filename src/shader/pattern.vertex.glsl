uniform mat4 u_matrix;

attribute vec2 a_pos;

varying vec2 v_pos;

void main() {
    v_pos = a_pos;
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}
