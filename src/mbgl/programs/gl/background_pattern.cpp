// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/programs/background_pattern_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<BackgroundPatternProgram> {
    static constexpr const char* name = "background_pattern";
    static constexpr const uint8_t hash[8] = { 0x70, 0x13, 0xc8, 0x7e, 0xba, 0x18, 0xf5, 0x19 };
    static constexpr const auto vertexOffset = 1675;
    static constexpr const auto fragmentOffset = 2266;
};

constexpr const char* ShaderSource<BackgroundPatternProgram>::name;
constexpr const uint8_t ShaderSource<BackgroundPatternProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<Program<BackgroundPatternProgram>>
Context::createProgram<gl::Context>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<BackgroundPatternProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of background_pattern.vertex.glsl:
/*
uniform mat4 u_matrix;
uniform vec2 u_pattern_size_a;
uniform vec2 u_pattern_size_b;
uniform vec2 u_pixel_coord_upper;
uniform vec2 u_pixel_coord_lower;
uniform float u_scale_a;
uniform float u_scale_b;
uniform float u_tile_units_to_pixels;

attribute vec2 a_pos;

varying vec2 v_pos_a;
varying vec2 v_pos_b;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);

    v_pos_a = get_pattern_pos(u_pixel_coord_upper, u_pixel_coord_lower, u_scale_a * u_pattern_size_a, u_tile_units_to_pixels, a_pos);
    v_pos_b = get_pattern_pos(u_pixel_coord_upper, u_pixel_coord_lower, u_scale_b * u_pattern_size_b, u_tile_units_to_pixels, a_pos);
}

*/

// Uncompressed source of background_pattern.fragment.glsl:
/*
uniform vec2 u_pattern_tl_a;
uniform vec2 u_pattern_br_a;
uniform vec2 u_pattern_tl_b;
uniform vec2 u_pattern_br_b;
uniform vec2 u_texsize;
uniform float u_mix;
uniform float u_opacity;

uniform sampler2D u_image;

varying vec2 v_pos_a;
varying vec2 v_pos_b;

void main() {
    vec2 imagecoord = mod(v_pos_a, 1.0);
    vec2 pos = mix(u_pattern_tl_a / u_texsize, u_pattern_br_a / u_texsize, imagecoord);
    vec4 color1 = texture2D(u_image, pos);

    vec2 imagecoord_b = mod(v_pos_b, 1.0);
    vec2 pos2 = mix(u_pattern_tl_b / u_texsize, u_pattern_br_b / u_texsize, imagecoord_b);
    vec4 color2 = texture2D(u_image, pos2);

    gl_FragColor = mix(color1, color2, u_mix) * u_opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/

