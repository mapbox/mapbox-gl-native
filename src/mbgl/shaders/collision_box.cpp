// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/collision_box.hpp>

namespace mbgl {
namespace shaders {

const char* collision_box::name = "collision_box";
const char* collision_box::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;
attribute vec2 a_extrude;
attribute vec2 a_data;

uniform mat4 u_matrix;
uniform float u_scale;

varying float v_max_zoom;
varying float v_placement_zoom;

void main() {
    gl_Position = u_matrix * vec4(a_pos + a_extrude / u_scale, 0.0, 1.0);

    v_max_zoom = a_data.x;
    v_placement_zoom = a_data.y;
}

)MBGL_SHADER";
const char* collision_box::fragmentSource = R"MBGL_SHADER(
uniform float u_zoom;
uniform float u_maxzoom;

varying float v_max_zoom;
varying float v_placement_zoom;

void main() {

    float alpha = 0.5;

    gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0) * alpha;

    if (v_placement_zoom > u_zoom) {
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * alpha;
    }

    if (u_zoom >= v_max_zoom) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0) * alpha * 0.25;
    }

    if (v_placement_zoom >= u_maxzoom) {
        gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0) * alpha * 0.2;
    }
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
