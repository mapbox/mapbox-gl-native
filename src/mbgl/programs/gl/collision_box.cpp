// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/collision_box_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<CollisionBoxProgram> {
    static constexpr const char* name = "collision_box";
    static constexpr const uint8_t hash[8] = {0xcb, 0x6a, 0x9b, 0xd1, 0x1f, 0x31, 0xf8, 0x5b};
    static constexpr const auto vertexOffset = 10000;
    static constexpr const auto fragmentOffset = 10679;
};

constexpr const char* ShaderSource<CollisionBoxProgram>::name;
constexpr const uint8_t ShaderSource<CollisionBoxProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<CollisionBoxProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<CollisionBoxProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of collision_box.vertex.glsl:
/*
attribute vec2 a_pos;
attribute vec2 a_anchor_pos;
attribute vec2 a_extrude;
attribute vec2 a_placed;
attribute vec2 a_shift;

uniform mat4 u_matrix;
uniform vec2 u_extrude_scale;
uniform float u_camera_to_center_distance;

varying float v_placed;
varying float v_notUsed;

void main() {
    vec4 projectedPoint = u_matrix * vec4(a_anchor_pos, 0, 1);
    highp float camera_to_anchor_distance = projectedPoint.w;
    highp float collision_perspective_ratio = clamp(
        0.5 + 0.5 * (u_camera_to_center_distance / camera_to_anchor_distance),
        0.0, // Prevents oversized near-field boxes in pitched/overzoomed tiles
        4.0);

    gl_Position = u_matrix * vec4(a_pos, 0.0, 1.0);
    gl_Position.xy += (a_extrude + a_shift) * u_extrude_scale * gl_Position.w * collision_perspective_ratio;

    v_placed = a_placed.x;
    v_notUsed = a_placed.y;
}

*/

// Uncompressed source of collision_box.fragment.glsl:
/*

varying float v_placed;
varying float v_notUsed;

void main() {

    float alpha = 0.5;

    // Red = collision, hide label
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * alpha;

    // Blue = no collision, label is showing
    if (v_placed > 0.5) {
        gl_FragColor = vec4(0.0, 0.0, 1.0, 0.5) * alpha;
    }

    if (v_notUsed > 0.5) {
        // This box not used, fade it out
        gl_FragColor *= .1;
    }
}
*/
// clang-format on
