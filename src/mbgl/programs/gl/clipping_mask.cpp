// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/programs/clipping_mask_program.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace gfx {

template <>
std::unique_ptr<Program<ClippingMaskProgram>>
Context::createProgram<gl::Context>(const ProgramParameters& programParameters) {
    return gl::Program<ClippingMaskProgram>::createProgram(
        reinterpret_cast<gl::Context&>(*this), programParameters, "clipping_mask",
        programs::gl::shaderSource() + 7848, programs::gl::shaderSource() + 7944);
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

