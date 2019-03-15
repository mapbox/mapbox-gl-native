// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/programs/heatmap_texture_program.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace gfx {

template <>
std::unique_ptr<Program<HeatmapTextureProgram>>
Context::createProgram<gl::Context>(const ProgramParameters& programParameters) {
    return gl::Program<HeatmapTextureProgram>::createProgram(
        reinterpret_cast<gl::Context&>(*this), programParameters, "heatmap_texture",
        programs::gl::shaderSource() + 9491, programs::gl::shaderSource() + 9671);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of heatmap_texture.vertex.glsl:
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

// Uncompressed source of heatmap_texture.fragment.glsl:
/*
uniform sampler2D u_image;
uniform sampler2D u_color_ramp;
uniform float u_opacity;
varying vec2 v_pos;

void main() {
    float t = texture2D(u_image, v_pos).r;
    vec4 color = texture2D(u_color_ramp, vec2(t, 0.5));
    gl_FragColor = color * u_opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(0.0);
#endif
}

*/

