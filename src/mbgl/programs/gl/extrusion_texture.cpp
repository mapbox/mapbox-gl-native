// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/programs/extrusion_texture_program.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace gfx {

template <>
std::unique_ptr<Program<ExtrusionTextureProgram>>
Context::createProgram<gl::Context>(const ProgramParameters& programParameters) {
    return gl::Program<ExtrusionTextureProgram>::createProgram(
        reinterpret_cast<gl::Context&>(*this), programParameters, "extrusion_texture",
        programs::gl::shaderSource() + 27554, programs::gl::shaderSource() + 27734);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of extrusion_texture.vertex.glsl:
/*
uniform mat4 u_matrix;
uniform vec2 u_world;
attribute vec2 a_pos;
varying vec2 v_pos;

void main() {
    gl_Position = u_matrix * vec4(a_pos * u_world, 0, 1);

    v_pos.x = a_pos.x;
    v_pos.y = 1.0 - a_pos.y;
}

*/

// Uncompressed source of extrusion_texture.fragment.glsl:
/*
uniform sampler2D u_image;
uniform float u_opacity;
varying vec2 v_pos;

void main() {
    gl_FragColor = texture2D(u_image, v_pos) * u_opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(0.0);
#endif
}

*/

