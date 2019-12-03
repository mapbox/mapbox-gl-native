// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/background_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<BackgroundProgram> {
    static constexpr const char* name = "background";
    static constexpr const uint8_t hash[8] = {0x2d, 0xef, 0x97, 0xa2, 0xec, 0xb5, 0x67, 0xef};
    static constexpr const auto vertexOffset = 1429;
    static constexpr const auto fragmentOffset = 1525;
};

constexpr const char* ShaderSource<BackgroundProgram>::name;
constexpr const uint8_t ShaderSource<BackgroundProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<BackgroundProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<BackgroundProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of background.vertex.glsl:
/*
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

*/

// Uncompressed source of background.fragment.glsl:
/*
uniform vec4 u_color;
uniform float u_opacity;

void main() {
    gl_FragColor = u_color * u_opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/
// clang-format on
