uniform mat4 u_matrix;

attribute vec2 a_pos;
attribute vec2 texCoord;

varying vec2 texCoordVar;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
    texCoordVar = texCoord;
}
