// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/programs/debug_program.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace gfx {

template <>
std::unique_ptr<Program<DebugProgram>>
Context::createProgram<gl::Context>(const ProgramParameters& programParameters) {
    return gl::Program<DebugProgram>::createProgram(
        reinterpret_cast<gl::Context&>(*this), programParameters, "debug",
        programs::gl::shaderSource() + 12450, programs::gl::shaderSource() + 12546);
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

