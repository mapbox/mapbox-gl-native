// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

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
    static constexpr const char* vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

)MBGL_SHADER";
    static constexpr const char* fragmentSource = R"MBGL_SHADER(
uniform vec4 u_color;
uniform float u_opacity;

void main() {
    gl_FragColor = u_color * u_opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";
};

constexpr const char* ShaderSource<BackgroundProgram>::name;
constexpr const char* ShaderSource<BackgroundProgram>::vertexSource;
constexpr const char* ShaderSource<BackgroundProgram>::fragmentSource;

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
