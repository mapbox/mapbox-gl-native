uniform sampler2D u_image;
uniform float u_opacity;

varying vec2 v_pos;

void main() {
    gl_FragColor = texture2D(u_image, v_pos, 2) * u_opacity;
}
