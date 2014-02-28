uniform sampler2D u_image;

varying vec2 v_pos;

void main() {
    gl_FragColor = texture2D(u_image, v_pos);
}
