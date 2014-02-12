uniform float u_debug;
uniform vec2 u_linewidth;
uniform vec4 u_color;

uniform vec2 u_dasharray;

varying vec2 v_normal;
varying float v_linesofar;

void main() {
    // Calculate the distance of the pixel from the line in pixels.
    float dist = length(v_normal) * u_linewidth.s;

    // Calculate the antialiasing fade factor. This is either when fading in
    // the line in case of an offset line (v_linewidth.t) or when fading out
    // (v_linewidth.s)
    float alpha = clamp(min(dist - (u_linewidth.t - 1.0), u_linewidth.s - dist), 0.0, 1.0);

    // Calculate the antialiasing fade factor based on distance to the dash.
    // Only affects alpha when line is dashed
    float pos = mod(v_linesofar, u_dasharray.x + u_dasharray.y);
    alpha *= max(step(0.0, -u_dasharray.y), clamp(min(pos, u_dasharray.x - pos), 0.0, 1.0));

    if (u_debug > 0.0) {
        gl_FragColor = u_color;
    } else {
        gl_FragColor = u_color * alpha;
    }
}