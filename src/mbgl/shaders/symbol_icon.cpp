// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/symbol_icon.hpp>

namespace mbgl {
namespace shaders {

const char* symbol_icon::name = "symbol_icon";
const char* symbol_icon::vertexSource = R"MBGL_SHADER(
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


// Encoding function
// static std::array<float, 2> encodeColor (const Color& color) {
//     const auto v1 = static_cast<uint16_t>( static_cast<uint16_t>(color.r*255)*256 + color.g*255);
//     const auto v2 = static_cast<uint16_t>( static_cast<uint16_t>(color.b*255)*256 + color.a*255);
//     return {{ static_cast<float>(v1), static_cast<float>(v2) }};
// }

vec4 decode_color(const vec2 encodedColor) {
    float r = floor(encodedColor[0]/256.0)/255.0;
    float g = (encodedColor[0] - r*256.0*255.0)/255.0;
    float b = floor(encodedColor[1]/256.0)/255.0;
    float a = (encodedColor[1] - b*256.0*255.0)/255.0;
    return vec4(r, g, b, a);
}

float unpack_mix_vec2(const vec2 packedValue, const float t) {
    return mix(packedValue[0], packedValue[1], t);
}

vec4 unpack_mix_vec4(const vec4 packedColors, const float t) {
    vec4 minColor = decode_color(vec2(packedColors[0], packedColors[1]));
    vec4 maxColor = decode_color(vec2(packedColors[2], packedColors[3]));
    return mix(minColor, maxColor, t);
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

#ifdef MAPBOX_GL_NATIVE
  attribute vec4 a_pos_offset;
#else
  attribute vec2 a_pos;
  attribute vec2 a_offset;
#endif

attribute vec2 a_texture_pos;
attribute vec4 a_data;


uniform lowp float a_opacity_t;
attribute lowp vec2 a_opacity_minmax;
varying lowp float opacity;

// matrix is for the vertex position.
uniform mat4 u_matrix;

uniform mediump float u_zoom;
uniform bool u_rotate_with_map;
uniform vec2 u_extrude_scale;

uniform vec2 u_texsize;

varying vec2 v_tex;
varying vec2 v_fade_tex;

void main() {
    opacity = unpack_mix_vec2(a_opacity_minmax, a_opacity_t);

#ifdef MAPBOX_GL_NATIVE
    mediump vec2 a_pos = a_pos_offset.xy;
    mediump vec2 a_offset = a_pos_offset.zw;
#endif

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
const char* symbol_icon::fragmentSource = R"MBGL_SHADER(
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

varying lowp float opacity;

varying vec2 v_tex;
varying vec2 v_fade_tex;

void main() {
    

    lowp float alpha = texture2D(u_fadetexture, v_fade_tex).a * opacity;
    gl_FragColor = texture2D(u_texture, v_tex) * alpha;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
