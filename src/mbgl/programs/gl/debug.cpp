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
    static constexpr const uint8_t hash[8] = {0xa8, 0x7d, 0x87, 0x6e, 0x36, 0xa8, 0x81, 0xe3};
    static constexpr const auto vertexOffset = 12494;
    static constexpr const auto fragmentOffset = 12590;
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

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

*/

// Uncompressed source of debug.fragment.glsl:
/*
uniform highp vec4 u_color;

void main() {
    gl_FragColor = u_color;
}

*/
// clang-format on
