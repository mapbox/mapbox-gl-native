// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/debug.hpp>

namespace mbgl {
namespace shaders {

const char* debug::name = "debug";
const char* debug::vertexSource = R"MBGL_SHADER(
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
//     return {{ static_cast<float>(color.r*256.0 + color.g),static_cast<float>( color.b*256.0 + color.a) }};
// }

vec4 decode_color(const vec2 encodedColor) {
    float r = floor(encodedColor[0]/256.0);
    float g = (encodedColor[0] - r*256.0);
    float b = floor(encodedColor[1]/256.0);
    float a = (encodedColor[1] - b*256.0);
    return vec4(r, g, b, a);
}

float unpack_mix_vec2(const vec2 packedValue, const float t) {
    return mix(packedValue[0], packedValue[1], t);
}

vec4 unpack_mix_vec4(const vec4 packedColors, const float t) {
    vec4 minColor = decode_color(packedColors.st);
    vec4 maxColor = decode_color(packedColors.pq);
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
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, step(32767.0, a_pos.x), 1);
}

)MBGL_SHADER";
const char* debug::fragmentSource = R"MBGL_SHADER(
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
uniform lowp vec4 u_color;

void main() {
    gl_FragColor = u_color;
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
