uniform vec4 u_color;
uniform float u_opacity;

void main() {
    gl_FragColor = u_color * u_opacity;
}
