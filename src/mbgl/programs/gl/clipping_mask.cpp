// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

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
    static constexpr const char* vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

)MBGL_SHADER";
    static constexpr const char* fragmentSource = R"MBGL_SHADER(
void main() {
    gl_FragColor = vec4(1.0);
}

)MBGL_SHADER";
};

constexpr const char* ShaderSource<ClippingMaskProgram>::name;
constexpr const char* ShaderSource<ClippingMaskProgram>::vertexSource;
constexpr const char* ShaderSource<ClippingMaskProgram>::fragmentSource;

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
