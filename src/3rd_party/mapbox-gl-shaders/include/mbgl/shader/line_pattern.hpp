#pragma once

// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/gl/gl.hpp>

namespace mbgl {
namespace shaders {

class line_pattern {
public:
    static constexpr const char* name = "line_pattern";
    static constexpr const char* vertexSource = R"MBGL_SHADER(
#ifdef GL_ES
precision highp float;
#else

#if !defined(lowp)
#define lowp
#endif

#if !defined(mediump)
#define mediump
#endif

#if !defined(highp)
#define highp
#endif

#endif

float evaluate_zoom_function_1(const vec4 values, const float t) {
    if (t < 1.0) {
        return mix(values[0], values[1], t);
    } else if (t < 2.0) {
        return mix(values[1], values[2], t - 1.0);
    } else {
        return mix(values[2], values[3], t - 2.0);
    }
}
vec4 evaluate_zoom_function_4(const vec4 value0, const vec4 value1, const vec4 value2, const vec4 value3, const float t) {
    if (t < 1.0) {
        return mix(value0, value1, t);
    } else if (t < 2.0) {
        return mix(value1, value2, t - 1.0);
    } else {
        return mix(value2, value3, t - 2.0);
    }
}

// The offset depends on how many pixels are between the world origin and the edge of the tile:
// vec2 offset = mod(pixel_coord, size)
//
// At high zoom levels there are a ton of pixels between the world origin and the edge of the tile.
// The glsl spec only guarantees 16 bits of precision for highp floats. We need more than that.
//
// The pixel_coord is passed in as two 16 bit values:
// pixel_coord_upper = floor(pixel_coord / 2^16)
// pixel_coord_lower = mod(pixel_coord, 2^16)
//
// The offset is calculated in a series of steps that should preserve this precision:
vec2 get_pattern_pos(const vec2 pixel_coord_upper, const vec2 pixel_coord_lower,
    const vec2 pattern_size, const float tile_units_to_pixels, const vec2 pos) {

    vec2 offset = mod(mod(mod(pixel_coord_upper, pattern_size) * 256.0, pattern_size) * 256.0 + pixel_coord_lower, pattern_size);
    return (tile_units_to_pixels * pos + offset) / pattern_size;
}
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

attribute vec2 a_pos;
attribute vec4 a_data;

uniform mat4 u_matrix;
uniform mediump float u_ratio;
uniform mediump float u_width;
uniform vec2 u_gl_units_to_pixels;

varying vec2 v_normal;
varying vec2 v_width2;
varying float v_linesofar;
varying float v_gamma_scale;

uniform lowp float u_blur;
uniform lowp float u_opacity;
uniform lowp float u_offset;
uniform mediump float u_gapwidth;

void main() {
    lowp float blur = u_blur;
    lowp float opacity = u_opacity;
    lowp float offset = u_offset;
    mediump float gapwidth = u_gapwidth;

    vec2 a_extrude = a_data.xy - 128.0;
    float a_direction = mod(a_data.z, 4.0) - 1.0;
    float a_linesofar = (floor(a_data.z / 4.0) + a_data.w * 64.0) * LINE_DISTANCE_SCALE;

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

    v_linesofar = a_linesofar;
    v_width2 = vec2(outset, inset);
}
)MBGL_SHADER";
    static constexpr const char* fragmentSource = R"MBGL_SHADER(
#ifdef GL_ES
precision mediump float;
#else

#if !defined(lowp)
#define lowp
#endif

#if !defined(mediump)
#define mediump
#endif

#if !defined(highp)
#define highp
#endif

#endif
uniform vec2 u_pattern_size_a;
uniform vec2 u_pattern_size_b;
uniform vec2 u_pattern_tl_a;
uniform vec2 u_pattern_br_a;
uniform vec2 u_pattern_tl_b;
uniform vec2 u_pattern_br_b;
uniform float u_fade;

uniform sampler2D u_image;

varying vec2 v_normal;
varying vec2 v_width2;
varying float v_linesofar;
varying float v_gamma_scale;

uniform lowp float u_blur;
uniform lowp float u_opacity;

void main() {
    lowp float blur = u_blur;
    lowp float opacity = u_opacity;

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
    vec2 pos_a = mix(u_pattern_tl_a, u_pattern_br_a, vec2(x_a, y_a));
    vec2 pos_b = mix(u_pattern_tl_b, u_pattern_br_b, vec2(x_b, y_b));

    vec4 color = mix(texture2D(u_image, pos_a), texture2D(u_image, pos_b), u_fade);

    gl_FragColor = color * alpha * opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}
)MBGL_SHADER";
};

} // namespace shaders
} // namespace mbgl
