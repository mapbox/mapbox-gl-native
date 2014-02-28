uniform sampler2D u_image;

varying vec2 texCoordVar;

void main() {
    gl_FragColor = texture2D(u_image, texCoordVar);
}
