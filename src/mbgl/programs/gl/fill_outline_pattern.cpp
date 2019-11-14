// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/fill_outline_pattern_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<FillOutlinePatternProgram> {
    static constexpr const char* name = "fill_outline_pattern";
    static constexpr const uint8_t hash[8] = {0x56, 0x9c, 0x2f, 0x58, 0x6b, 0x31, 0xff, 0x84};
    static constexpr const auto vertexOffset = 15137;
    static constexpr const auto fragmentOffset = 16997;
};

constexpr const char* ShaderSource<FillOutlinePatternProgram>::name;
constexpr const uint8_t ShaderSource<FillOutlinePatternProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<FillOutlinePatternProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<FillOutlinePatternProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of fill_outline_pattern.vertex.glsl:
/*
uniform mat4 u_matrix;
uniform vec2 u_world;
uniform vec2 u_pixel_coord_upper;
uniform vec2 u_pixel_coord_lower;
uniform vec4 u_scale;

attribute vec2 a_pos;

varying vec2 v_pos_a;
varying vec2 v_pos_b;
varying vec2 v_pos;


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
    float tileRatio = u_scale.y;
    float fromScale = u_scale.z;
    float toScale = u_scale.w;

    gl_Position = u_matrix * vec4(a_pos, 0, 1);

    vec2 display_size_a = vec2((pattern_br_a.x - pattern_tl_a.x) / pixelRatio, (pattern_br_a.y - pattern_tl_a.y) / pixelRatio);
    vec2 display_size_b = vec2((pattern_br_b.x - pattern_tl_b.x) / pixelRatio, (pattern_br_b.y - pattern_tl_b.y) / pixelRatio);

    v_pos_a = get_pattern_pos(u_pixel_coord_upper, u_pixel_coord_lower, fromScale * display_size_a, tileRatio, a_pos);
    v_pos_b = get_pattern_pos(u_pixel_coord_upper, u_pixel_coord_lower, toScale * display_size_b, tileRatio, a_pos);

    v_pos = (gl_Position.xy / gl_Position.w + 1.0) / 2.0 * u_world;
}

*/

// Uncompressed source of fill_outline_pattern.fragment.glsl:
/*

uniform vec2 u_texsize;
uniform sampler2D u_image;
uniform float u_fade;

varying vec2 v_pos_a;
varying vec2 v_pos_b;
varying vec2 v_pos;


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

    // find distance to outline for alpha interpolation

    float dist = length(v_pos - gl_FragCoord.xy);
    float alpha = 1.0 - smoothstep(0.0, 1.0, dist);


    gl_FragColor = mix(color1, color2, u_fade) * alpha * opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/
// clang-format on
