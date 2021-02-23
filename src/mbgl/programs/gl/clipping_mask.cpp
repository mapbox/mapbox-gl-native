// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/clipping_mask_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<ClippingMaskProgram> {
    static constexpr const char* name = "clipping_mask";
    static constexpr const uint8_t hash[8] = {0x3e, 0x17, 0xc2, 0x3a, 0x1f, 0xf0, 0xa8, 0xa3};
    static constexpr const auto vertexOffset = 7891;
    static constexpr const auto fragmentOffset = 7987;
};

constexpr const char* ShaderSource<ClippingMaskProgram>::name;
constexpr const uint8_t ShaderSource<ClippingMaskProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<ClippingMaskProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<ClippingMaskProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of clipping_mask.vertex.glsl:
/*
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

*/

// Uncompressed source of clipping_mask.fragment.glsl:
/*
void main() {
    gl_FragColor = vec4(1.0);
}

*/
// clang-format on
