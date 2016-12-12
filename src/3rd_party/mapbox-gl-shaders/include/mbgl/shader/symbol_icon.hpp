#pragma once

// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/gl/gl.hpp>

namespace mbgl {
namespace shaders {

class symbol_icon {
public:
    static constexpr const char* name = "symbol_icon";
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
attribute vec2 a_pos;
attribute vec2 a_offset;
attribute vec2 a_texture_pos;
attribute vec4 a_data;


// matrix is for the vertex position.
uniform mat4 u_matrix;

uniform mediump float u_zoom;
uniform bool u_rotate_with_map;
uniform vec2 u_extrude_scale;

uniform vec2 u_texsize;

varying vec2 v_tex;
varying vec2 v_fade_tex;

void main() {
    vec2 a_tex = a_texture_pos.xy;
    mediump float a_labelminzoom = a_data[0];
    mediump vec2 a_zoom = a_data.pq;
    mediump float a_minzoom = a_zoom[0];
    mediump float a_maxzoom = a_zoom[1];

    // u_zoom is the current zoom level adjusted for the change in font size
    mediump float z = 2.0 - step(a_minzoom, u_zoom) - (1.0 - step(a_maxzoom, u_zoom));

    vec2 extrude = u_extrude_scale * (a_offset / 64.0);
    if (u_rotate_with_map) {
        gl_Position = u_matrix * vec4(a_pos + extrude, 0, 1);
        gl_Position.z += z * gl_Position.w;
    } else {
        gl_Position = u_matrix * vec4(a_pos, 0, 1) + vec4(extrude, 0, 0);
    }

    v_tex = a_tex / u_texsize;
    v_fade_tex = vec2(a_labelminzoom / 255.0, 0.0);
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
uniform sampler2D u_texture;
uniform sampler2D u_fadetexture;
uniform lowp float u_opacity;

varying vec2 v_tex;
varying vec2 v_fade_tex;

void main() {
    lowp float alpha = texture2D(u_fadetexture, v_fade_tex).a * u_opacity;
    gl_FragColor = texture2D(u_texture, v_tex) * alpha;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}
)MBGL_SHADER";
};

} // namespace shaders
} // namespace mbgl
