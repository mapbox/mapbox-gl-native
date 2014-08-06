uniform sampler2D u_texture;

varying vec2 v_tex;
varying float v_alpha;

void main() {
    // Note: We don't need to premultiply here as the image we use is already
    // premultiplied in the sprite atlas.
    gl_FragColor = texture2D(u_texture, v_tex) * v_alpha;
}
