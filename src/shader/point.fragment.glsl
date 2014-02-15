#define root2 1.42

uniform sampler2D u_image;
uniform vec2 u_tl;
uniform vec2 u_br;
uniform vec4 u_color;

uniform vec2 pos;
uniform float inbounds;
uniform vec4 color;

void main() {
    vec2 pos = (gl_PointCoord * 2.0 - 1.0) * root2 / 2.0 + 0.5;

    float inbounds = step(0.0, pos.x) * step(0.0, pos.y) *
        (1.0 - step(1.0, pos.x)) * (1.0 - step(1.0, pos.y));

    vec4 color = texture2D(u_image, mix(u_tl, u_br, pos)) * inbounds;

    if (u_color.a > 0.0) {
        gl_FragColor = u_color * color.a;
    } else {
        gl_FragColor = vec4(color.rgb * color.a, color.a);
    }
}
