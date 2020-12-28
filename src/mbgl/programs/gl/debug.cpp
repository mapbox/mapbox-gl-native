// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/debug_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<DebugProgram> {
    static constexpr const char* name = "debug";
    static constexpr const uint8_t hash[8] = {0x07, 0x98, 0x41, 0xa8, 0x6b, 0x73, 0xaf, 0x34};
    static constexpr const auto vertexOffset = 12494;
    static constexpr const auto fragmentOffset = 12672;
};

constexpr const char* ShaderSource<DebugProgram>::name;
constexpr const uint8_t ShaderSource<DebugProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<DebugProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<DebugProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of debug.vertex.glsl:
/*
attribute vec2 a_pos;
varying vec2 v_uv;

uniform mat4 u_matrix;
uniform float u_overlay_scale;

void main() {
    // This vertex shader expects a EXTENT x EXTENT quad,
    // The UV co-ordinates for the overlay texture can be calculated using that knowledge
    v_uv = a_pos / 8192.0;
    gl_Position = u_matrix * vec4(a_pos * u_overlay_scale, 0, 1);
}

*/

// Uncompressed source of debug.fragment.glsl:
/*
uniform highp vec4 u_color;
uniform sampler2D u_overlay;

varying vec2 v_uv;

void main() {
    vec4 overlay_color = texture2D(u_overlay, v_uv);
    gl_FragColor = mix(u_color, overlay_color, overlay_color.a);
}

*/
// clang-format on
