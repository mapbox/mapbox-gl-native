// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/fill_outline_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<FillOutlineProgram> {
    static constexpr const char* name = "fill_outline";
    static constexpr const uint8_t hash[8] = {0x51, 0x25, 0x43, 0x9d, 0x41, 0x73, 0xe1, 0xbb};
    static constexpr const auto vertexOffset = 13722;
    static constexpr const auto fragmentOffset = 14547;
};

constexpr const char* ShaderSource<FillOutlineProgram>::name;
constexpr const uint8_t ShaderSource<FillOutlineProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<FillOutlineProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<FillOutlineProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of fill_outline.vertex.glsl:
/*
attribute vec2 a_pos;

uniform mat4 u_matrix;
uniform vec2 u_world;

varying vec2 v_pos;


#ifndef HAS_UNIFORM_u_outline_color
uniform lowp float u_outline_color_t;
attribute highp vec4 a_outline_color;
varying highp vec4 outline_color;
#else
uniform highp vec4 u_outline_color;
#endif


#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


void main() {
    
#ifndef HAS_UNIFORM_u_outline_color
    outline_color = unpack_mix_color(a_outline_color, u_outline_color_t);
#else
    highp vec4 outline_color = u_outline_color;
#endif

    
#ifndef HAS_UNIFORM_u_opacity
    opacity = unpack_mix_vec2(a_opacity, u_opacity_t);
#else
    lowp float opacity = u_opacity;
#endif


    gl_Position = u_matrix * vec4(a_pos, 0, 1);
    v_pos = (gl_Position.xy / gl_Position.w + 1.0) / 2.0 * u_world;
}

*/

// Uncompressed source of fill_outline.fragment.glsl:
/*
varying vec2 v_pos;


#ifndef HAS_UNIFORM_u_outline_color
varying highp vec4 outline_color;
#else
uniform highp vec4 u_outline_color;
#endif


#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


void main() {
    
#ifdef HAS_UNIFORM_u_outline_color
    highp vec4 outline_color = u_outline_color;
#endif

    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif


    float dist = length(v_pos - gl_FragCoord.xy);
    float alpha = 1.0 - smoothstep(0.0, 1.0, dist);
    gl_FragColor = outline_color * (alpha * opacity);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/
// clang-format on
