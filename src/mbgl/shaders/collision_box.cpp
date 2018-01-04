// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/collision_box.hpp>

namespace mbgl {
namespace shaders {

const char* collision_box::name = "collision_box";
const char* collision_box::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;
attribute vec2 a_anchor_pos;
attribute vec2 a_extrude;
attribute vec2 a_placed;

uniform mat4 u_matrix;
uniform vec2 u_extrude_scale;
uniform float u_camera_to_center_distance;

varying float v_placed;
varying float v_notUsed;

void main() {
    vec4 projectedPoint = u_matrix * vec4(a_anchor_pos, 0, 1);
    highp float camera_to_anchor_distance = projectedPoint.w;
    highp float collision_perspective_ratio = 0.5 + 0.5 * (u_camera_to_center_distance / camera_to_anchor_distance);

    gl_Position = u_matrix * vec4(a_pos, 0.0, 1.0);
    gl_Position.xy += a_extrude * u_extrude_scale * gl_Position.w * collision_perspective_ratio;

    v_placed = a_placed.x;
    v_notUsed = a_placed.y;
}

)MBGL_SHADER";
const char* collision_box::fragmentSource = R"MBGL_SHADER(

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
)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
