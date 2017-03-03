// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/circle.hpp>

namespace mbgl {
namespace shaders {

const char* circle::name = "circle";
const char* circle::vertexSource = R"MBGL_SHADER(
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
uniform mat4 u_matrix;
uniform bool u_scale_with_map;
uniform vec2 u_extrude_scale;

attribute vec2 a_pos;

uniform lowp float a_color_t;
attribute lowp vec4 a_color_minmax;
varying lowp vec4 color;
uniform lowp float a_radius_t;
attribute mediump vec2 a_radius_minmax;
varying mediump float radius;
uniform lowp float a_blur_t;
attribute lowp vec2 a_blur_minmax;
varying lowp float blur;
uniform lowp float a_opacity_t;
attribute lowp vec2 a_opacity_minmax;
varying lowp float opacity;
uniform lowp float a_stroke_color_t;
attribute lowp vec4 a_stroke_color_minmax;
varying lowp vec4 stroke_color;
uniform lowp float a_stroke_width_t;
attribute mediump vec2 a_stroke_width_minmax;
varying mediump float stroke_width;
uniform lowp float a_stroke_opacity_t;
attribute lowp vec2 a_stroke_opacity_minmax;
varying lowp float stroke_opacity;

varying vec2 v_extrude;
varying lowp float v_antialiasblur;

void main(void) {
    color = unpack_mix_vec4(a_color_minmax, a_color_t);
    radius = unpack_mix_vec2(a_radius_minmax, a_radius_t);
    blur = unpack_mix_vec2(a_blur_minmax, a_blur_t);
    opacity = unpack_mix_vec2(a_opacity_minmax, a_opacity_t);
    stroke_color = unpack_mix_vec4(a_stroke_color_minmax, a_stroke_color_t);
    stroke_width = unpack_mix_vec2(a_stroke_width_minmax, a_stroke_width_t);
    stroke_opacity = unpack_mix_vec2(a_stroke_opacity_minmax, a_stroke_opacity_t);

    // unencode the extrusion vector that we snuck into the a_pos vector
    v_extrude = vec2(mod(a_pos, 2.0) * 2.0 - 1.0);

    vec2 extrude = v_extrude * (radius + stroke_width) * u_extrude_scale;
    // multiply a_pos by 0.5, since we had it * 2 in order to sneak
    // in extrusion data
    gl_Position = u_matrix * vec4(floor(a_pos * 0.5), 0, 1);

    if (u_scale_with_map) {
        gl_Position.xy += extrude;
    } else {
        gl_Position.xy += extrude * gl_Position.w;
    }

    // This is a minimum blur distance that serves as a faux-antialiasing for
    // the circle. since blur is a ratio of the circle's size and the intent is
    // to keep the blur at roughly 1px, the two are inversely related.
    v_antialiasblur = 1.0 / DEVICE_PIXEL_RATIO / (radius + stroke_width);
}

)MBGL_SHADER";
const char* circle::fragmentSource = R"MBGL_SHADER(
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
varying lowp vec4 color;
varying mediump float radius;
varying lowp float blur;
varying lowp float opacity;
varying lowp vec4 stroke_color;
varying mediump float stroke_width;
varying lowp float stroke_opacity;

varying vec2 v_extrude;
varying lowp float v_antialiasblur;

void main() {
    
    
    
    
    
    
    

    float extrude_length = length(v_extrude);
    float antialiased_blur = -max(blur, v_antialiasblur);

    float opacity_t = smoothstep(0.0, antialiased_blur, extrude_length - 1.0);

    float color_t = stroke_width < 0.01 ? 0.0 : smoothstep(
        antialiased_blur,
        0.0,
        extrude_length - radius / (radius + stroke_width)
    );

    gl_FragColor = opacity_t * mix(color * opacity, stroke_color * stroke_opacity, color_t);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
