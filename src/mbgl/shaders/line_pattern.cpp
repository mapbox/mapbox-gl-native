// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/line_pattern.hpp>

namespace mbgl {
namespace shaders {

const char* line_pattern::name = "line_pattern";
const char* line_pattern::vertexSource = R"MBGL_SHADER(
// floor(127 / 2) == 63.0
// the maximum allowed miter limit is 2.0 at the moment. the extrude normal is
// stored in a byte (-128..127). we scale regular normals up to length 63, but
// there are also "special" normals that have a bigger length (of up to 126 in
// this case).
// #define scale 63.0
#define scale 0.015873016

// We scale the distance before adding it to the buffers so that we can store
// long distances for long segments. Use this value to unscale the distance.
#define LINE_DISTANCE_SCALE 2.0

// the distance over which the line edge fades out.
// Retina devices need a smaller distance to avoid aliasing.
#define ANTIALIASING 1.0 / DEVICE_PIXEL_RATIO / 2.0

attribute vec4 a_pos_normal;
attribute vec4 a_data;

uniform mat4 u_matrix;
uniform mediump float u_ratio;
uniform vec2 u_gl_units_to_pixels;

varying vec2 v_normal;
varying vec2 v_width2;
varying float v_linesofar;
varying float v_gamma_scale;


#ifndef HAS_UNIFORM_u_blur
uniform lowp float a_blur_t;
attribute lowp vec2 a_blur;
varying lowp float blur;
#else
uniform lowp float u_blur;
#endif


#ifndef HAS_UNIFORM_u_opacity
uniform lowp float a_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


#ifndef HAS_UNIFORM_u_offset
uniform lowp float a_offset_t;
attribute lowp vec2 a_offset;
#else
uniform lowp float u_offset;
#endif


#ifndef HAS_UNIFORM_u_gapwidth
uniform lowp float a_gapwidth_t;
attribute mediump vec2 a_gapwidth;
#else
uniform mediump float u_gapwidth;
#endif


#ifndef HAS_UNIFORM_u_width
uniform lowp float a_width_t;
attribute mediump vec2 a_width;
#else
uniform mediump float u_width;
#endif


void main() {
    
#ifndef HAS_UNIFORM_u_blur
    blur = unpack_mix_vec2(a_blur, a_blur_t);
#else
    lowp float blur = u_blur;
#endif

    
#ifndef HAS_UNIFORM_u_opacity
    opacity = unpack_mix_vec2(a_opacity, a_opacity_t);
#else
    lowp float opacity = u_opacity;
#endif

    
#ifndef HAS_UNIFORM_u_offset
    lowp float offset = unpack_mix_vec2(a_offset, a_offset_t);
#else
    lowp float offset = u_offset;
#endif

    
#ifndef HAS_UNIFORM_u_gapwidth
    mediump float gapwidth = unpack_mix_vec2(a_gapwidth, a_gapwidth_t);
#else
    mediump float gapwidth = u_gapwidth;
#endif

    
#ifndef HAS_UNIFORM_u_width
    mediump float width = unpack_mix_vec2(a_width, a_width_t);
#else
    mediump float width = u_width;
#endif


    vec2 a_extrude = a_data.xy - 128.0;
    float a_direction = mod(a_data.z, 4.0) - 1.0;
    float a_linesofar = (floor(a_data.z / 4.0) + a_data.w * 64.0) * LINE_DISTANCE_SCALE;

    vec2 pos = a_pos_normal.xy;

    // x is 1 if it's a round cap, 0 otherwise
    // y is 1 if the normal points up, and -1 if it points down
    mediump vec2 normal = a_pos_normal.zw;
    v_normal = normal;

    // these transformations used to be applied in the JS and native code bases.
    // moved them into the shader for clarity and simplicity.
    gapwidth = gapwidth / 2.0;
    float halfwidth = width / 2.0;
    offset = -1.0 * offset;

    float inset = gapwidth + (gapwidth > 0.0 ? ANTIALIASING : 0.0);
    float outset = gapwidth + halfwidth * (gapwidth > 0.0 ? 2.0 : 1.0) + ANTIALIASING;

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

    vec4 projected_extrude = u_matrix * vec4(dist / u_ratio, 0.0, 0.0);
    gl_Position = u_matrix * vec4(pos + offset2 / u_ratio, 0.0, 1.0) + projected_extrude;

    // calculate how much the perspective view squishes or stretches the extrude
    float extrude_length_without_perspective = length(dist);
    float extrude_length_with_perspective = length(projected_extrude.xy / gl_Position.w * u_gl_units_to_pixels);
    v_gamma_scale = extrude_length_without_perspective / extrude_length_with_perspective;

    v_linesofar = a_linesofar;
    v_width2 = vec2(outset, inset);
}

)MBGL_SHADER";
const char* line_pattern::fragmentSource = R"MBGL_SHADER(
uniform vec2 u_pattern_size_a;
uniform vec2 u_pattern_size_b;
uniform vec2 u_pattern_tl_a;
uniform vec2 u_pattern_br_a;
uniform vec2 u_pattern_tl_b;
uniform vec2 u_pattern_br_b;
uniform vec2 u_texsize;
uniform float u_fade;

uniform sampler2D u_image;

varying vec2 v_normal;
varying vec2 v_width2;
varying float v_linesofar;
varying float v_gamma_scale;


#ifndef HAS_UNIFORM_u_blur
varying lowp float blur;
#else
uniform lowp float u_blur;
#endif


#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


void main() {
    
#ifdef HAS_UNIFORM_u_blur
    lowp float blur = u_blur;
#endif

    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif


    // Calculate the distance of the pixel from the line in pixels.
    float dist = length(v_normal) * v_width2.s;

    // Calculate the antialiasing fade factor. This is either when fading in
    // the line in case of an offset line (v_width2.t) or when fading out
    // (v_width2.s)
    float blur2 = (blur + 1.0 / DEVICE_PIXEL_RATIO) * v_gamma_scale;
    float alpha = clamp(min(dist - (v_width2.t - blur2), v_width2.s - dist) / blur2, 0.0, 1.0);

    float x_a = mod(v_linesofar / u_pattern_size_a.x, 1.0);
    float x_b = mod(v_linesofar / u_pattern_size_b.x, 1.0);
    float y_a = 0.5 + (v_normal.y * v_width2.s / u_pattern_size_a.y);
    float y_b = 0.5 + (v_normal.y * v_width2.s / u_pattern_size_b.y);
    vec2 pos_a = mix(u_pattern_tl_a / u_texsize, u_pattern_br_a / u_texsize, vec2(x_a, y_a));
    vec2 pos_b = mix(u_pattern_tl_b / u_texsize, u_pattern_br_b / u_texsize, vec2(x_b, y_b));

    vec4 color = mix(texture2D(u_image, pos_a), texture2D(u_image, pos_b), u_fade);

    gl_FragColor = color * alpha * opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
