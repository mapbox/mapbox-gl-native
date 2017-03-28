// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/line.hpp>

namespace mbgl {
namespace shaders {

const char* line::name = "line";
const char* line::vertexSource = R"MBGL_SHADER(


// the distance over which the line edge fades out.
// Retina devices need a smaller distance to avoid aliasing.
#define ANTIALIASING 1.0 / DEVICE_PIXEL_RATIO / 2.0

// floor(127 / 2) == 63.0
// the maximum allowed miter limit is 2.0 at the moment. the extrude normal is
// stored in a byte (-128..127). we scale regular normals up to length 63, but
// there are also "special" normals that have a bigger length (of up to 126 in
// this case).
// #define scale 63.0
#define scale 0.015873016

attribute vec2 a_pos;
attribute vec4 a_data;

uniform mat4 u_matrix;
uniform mediump float u_ratio;
uniform mediump float u_width;
uniform vec2 u_gl_units_to_pixels;

varying vec2 v_normal;
varying vec2 v_width2;
varying float v_gamma_scale;

uniform lowp float a_color_t;
attribute highp vec4 a_color;
varying highp vec4 color;
uniform lowp float a_blur_t;
attribute lowp vec2 a_blur;
varying lowp float blur;
uniform lowp float a_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
uniform lowp float a_gapwidth_t;
attribute mediump vec2 a_gapwidth;
varying mediump float gapwidth;
uniform lowp float a_offset_t;
attribute lowp vec2 a_offset;
varying lowp float offset;

void main() {
    color = unpack_mix_vec4(a_color, a_color_t);
    blur = unpack_mix_vec2(a_blur, a_blur_t);
    opacity = unpack_mix_vec2(a_opacity, a_opacity_t);
    gapwidth = unpack_mix_vec2(a_gapwidth, a_gapwidth_t);
    offset = unpack_mix_vec2(a_offset, a_offset_t);

    vec2 a_extrude = a_data.xy - 128.0;
    float a_direction = mod(a_data.z, 4.0) - 1.0;

    // We store the texture normals in the most insignificant bit
    // transform y so that 0 => -1 and 1 => 1
    // In the texture normal, x is 0 if the normal points straight up/down and 1 if it's a round cap
    // y is 1 if the normal points up, and -1 if it points down
    mediump vec2 normal = mod(a_pos, 2.0);
    normal.y = sign(normal.y - 0.5);
    v_normal = normal;


    // these transformations used to be applied in the JS and native code bases. 
    // moved them into the shader for clarity and simplicity. 
    gapwidth = gapwidth / 2.0;
    float width = u_width / 2.0;
    offset = -1.0 * offset; 

    float inset = gapwidth + (gapwidth > 0.0 ? ANTIALIASING : 0.0);
    float outset = gapwidth + width * (gapwidth > 0.0 ? 2.0 : 1.0) + ANTIALIASING;

    // Scale the extrusion vector down to a normal and then up by the line width
    // of this vertex.
    mediump vec2 dist = outset * a_extrude * scale;

    // Calculate the offset when drawing a line that is to the side of the actual line.
    // We do this by creating a vector that points towards the extrude, but rotate
    // it when we're drawing round end points (a_direction = -1 or 1) since their
    // extrude vector points in another direction.
    mediump float u = 0.5 * a_direction;
    mediump float t = 1.0 - abs(u);
    mediump vec2 offset2 = offset * a_extrude * scale * normal.y * mat2(t, -u, u, t);

    // Remove the texture normal bit to get the position
    vec2 pos = floor(a_pos * 0.5);

    vec4 projected_extrude = u_matrix * vec4(dist / u_ratio, 0.0, 0.0);
    gl_Position = u_matrix * vec4(pos + offset2 / u_ratio, 0.0, 1.0) + projected_extrude;

    // calculate how much the perspective view squishes or stretches the extrude
    float extrude_length_without_perspective = length(dist);
    float extrude_length_with_perspective = length(projected_extrude.xy / gl_Position.w * u_gl_units_to_pixels);
    v_gamma_scale = extrude_length_without_perspective / extrude_length_with_perspective;

    v_width2 = vec2(outset, inset);
}

)MBGL_SHADER";
const char* line::fragmentSource = R"MBGL_SHADER(
varying highp vec4 color;
varying lowp float blur;
varying lowp float opacity;

varying vec2 v_width2;
varying vec2 v_normal;
varying float v_gamma_scale;

void main() {
    
    
    

    // Calculate the distance of the pixel from the line in pixels.
    float dist = length(v_normal) * v_width2.s;

    // Calculate the antialiasing fade factor. This is either when fading in
    // the line in case of an offset line (v_width2.t) or when fading out
    // (v_width2.s)
    float blur2 = (blur + 1.0 / DEVICE_PIXEL_RATIO) * v_gamma_scale;
    float alpha = clamp(min(dist - (v_width2.t - blur2), v_width2.s - dist) / blur2, 0.0, 1.0);

    gl_FragColor = color * (alpha * opacity);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
