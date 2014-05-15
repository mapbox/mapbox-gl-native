uniform sampler2D u_image;
uniform vec2 u_pos;
uniform vec2 u_dimension;
uniform vec4 u_color;
uniform float u_size;

void main() {
    vec2 pos = u_pos + (gl_PointCoord - 0.5) * u_size / u_dimension;
    gl_FragColor = texture2D(u_image, pos) * u_color;
}
