// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/fill_pattern_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<FillPatternProgram> {
    static constexpr const char* name = "fill_pattern";
    static constexpr const uint8_t hash[8] = {0x74, 0xa9, 0x97, 0x01, 0x96, 0xbd, 0x87, 0x36};
    static constexpr const auto vertexOffset = 18304;
    static constexpr const auto fragmentOffset = 20083;
};

constexpr const char* ShaderSource<FillPatternProgram>::name;
constexpr const uint8_t ShaderSource<FillPatternProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<FillPatternProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<FillPatternProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of fill_pattern.vertex.glsl:
/*
uniform mat4 u_matrix;
uniform vec2 u_pixel_coord_upper;
uniform vec2 u_pixel_coord_lower;
uniform vec4 u_scale;

attribute vec2 a_pos;

varying vec2 v_pos_a;
varying vec2 v_pos_b;


#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


#ifndef HAS_UNIFORM_u_pattern_from
uniform lowp float u_pattern_from_t;
attribute lowp vec4 a_pattern_from;
varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif


#ifndef HAS_UNIFORM_u_pattern_to
uniform lowp float u_pattern_to_t;
attribute lowp vec4 a_pattern_to;
varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
#endif


void main() {
    
#ifndef HAS_UNIFORM_u_opacity
    opacity = unpack_mix_vec2(a_opacity, u_opacity_t);
#else
    lowp float opacity = u_opacity;
#endif

    
#ifndef HAS_UNIFORM_u_pattern_from
    pattern_from = a_pattern_from;
#else
    mediump vec4 pattern_from = u_pattern_from;
#endif

    
#ifndef HAS_UNIFORM_u_pattern_to
    pattern_to = a_pattern_to;
#else
    mediump vec4 pattern_to = u_pattern_to;
#endif


    vec2 pattern_tl_a = pattern_from.xy;
    vec2 pattern_br_a = pattern_from.zw;
    vec2 pattern_tl_b = pattern_to.xy;
    vec2 pattern_br_b = pattern_to.zw;

    float pixelRatio = u_scale.x;
    float tileZoomRatio = u_scale.y;
    float fromScale = u_scale.z;
    float toScale = u_scale.w;

    vec2 display_size_a = vec2((pattern_br_a.x - pattern_tl_a.x) / pixelRatio, (pattern_br_a.y - pattern_tl_a.y) / pixelRatio);
    vec2 display_size_b = vec2((pattern_br_b.x - pattern_tl_b.x) / pixelRatio, (pattern_br_b.y - pattern_tl_b.y) / pixelRatio);
    gl_Position = u_matrix * vec4(a_pos, 0, 1);

    v_pos_a = get_pattern_pos(u_pixel_coord_upper, u_pixel_coord_lower, fromScale * display_size_a, tileZoomRatio, a_pos);
    v_pos_b = get_pattern_pos(u_pixel_coord_upper, u_pixel_coord_lower, toScale * display_size_b, tileZoomRatio, a_pos);
}

*/

// Uncompressed source of fill_pattern.fragment.glsl:
/*
uniform vec2 u_texsize;
uniform float u_fade;

uniform sampler2D u_image;

varying vec2 v_pos_a;
varying vec2 v_pos_b;


#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


#ifndef HAS_UNIFORM_u_pattern_from
varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif


#ifndef HAS_UNIFORM_u_pattern_to
varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
#endif


void main() {
    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif

    
#ifdef HAS_UNIFORM_u_pattern_from
    mediump vec4 pattern_from = u_pattern_from;
#endif

    
#ifdef HAS_UNIFORM_u_pattern_to
    mediump vec4 pattern_to = u_pattern_to;
#endif


    vec2 pattern_tl_a = pattern_from.xy;
    vec2 pattern_br_a = pattern_from.zw;
    vec2 pattern_tl_b = pattern_to.xy;
    vec2 pattern_br_b = pattern_to.zw;

    vec2 imagecoord = mod(v_pos_a, 1.0);
    vec2 pos = mix(pattern_tl_a / u_texsize, pattern_br_a / u_texsize, imagecoord);
    vec4 color1 = texture2D(u_image, pos);

    vec2 imagecoord_b = mod(v_pos_b, 1.0);
    vec2 pos2 = mix(pattern_tl_b / u_texsize, pattern_br_b / u_texsize, imagecoord_b);
    vec4 color2 = texture2D(u_image, pos2);

    gl_FragColor = mix(color1, color2, u_fade) * opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/
// clang-format on
