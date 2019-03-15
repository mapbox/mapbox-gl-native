// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/programs/background_program.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace gfx {

template <>
std::unique_ptr<Program<BackgroundProgram>>
Context::createProgram<gl::Context>(const ProgramParameters& programParameters) {
    return gl::Program<BackgroundProgram>::createProgram(
        reinterpret_cast<gl::Context&>(*this), programParameters, "background",
        programs::gl::shaderSource() + 1429, programs::gl::shaderSource() + 1525);
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

