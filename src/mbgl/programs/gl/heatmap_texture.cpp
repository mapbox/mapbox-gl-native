// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/heatmap_texture_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<HeatmapTextureProgram> {
    static constexpr const char* name = "heatmap_texture";
    static constexpr const uint8_t hash[8] = {0x9f, 0xc7, 0x56, 0xb2, 0x9e, 0x8f, 0x15, 0xff};
    static constexpr const auto vertexOffset = 9535;
    static constexpr const auto fragmentOffset = 9715;
};

constexpr const char* ShaderSource<HeatmapTextureProgram>::name;
constexpr const uint8_t ShaderSource<HeatmapTextureProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<HeatmapTextureProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<HeatmapTextureProgram>>(programParameters);
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
// clang-format on
