// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/collision_box.hpp>

namespace mbgl {
namespace shaders {

const char* collision_box::name = "collision_box";
const char* collision_box::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;
attribute vec2 a_anchor_pos;
attribute vec2 a_extrude;
attribute vec2 a_data;

uniform mat4 u_matrix;
uniform float u_scale;
uniform float u_pitch;
uniform float u_collision_y_stretch;
uniform float u_camera_to_center_distance;

varying float v_max_zoom;
varying float v_placement_zoom;
varying float v_perspective_zoom_adjust;
varying vec2 v_fade_tex;

void main() {
    vec4 projectedPoint = u_matrix * vec4(a_anchor_pos, 0, 1);
    highp float camera_to_anchor_distance = projectedPoint.w;
    highp float collision_perspective_ratio = 1.0 + 0.5 * ((camera_to_anchor_distance / u_camera_to_center_distance) - 1.0);

    highp float incidence_stretch  = camera_to_anchor_distance / (u_camera_to_center_distance * cos(u_pitch));
    highp float collision_adjustment = max(1.0, incidence_stretch / u_collision_y_stretch);

    gl_Position = u_matrix * vec4(a_pos + a_extrude * collision_perspective_ratio * collision_adjustment / u_scale, 0.0, 1.0);

    v_max_zoom = a_data.x;
    v_placement_zoom = a_data.y;

    v_perspective_zoom_adjust = floor(log2(collision_perspective_ratio * collision_adjustment) * 10.0);
    v_fade_tex = vec2((v_placement_zoom + v_perspective_zoom_adjust) / 255.0, 0.0);
}

)MBGL_SHADER";
const char* collision_box::fragmentSource = R"MBGL_SHADER(
uniform float u_zoom;
// u_maxzoom is derived from the maximum scale considered by the CollisionTile
// Labels with placement zoom greater than this value will not be placed,
// regardless of perspective effects.
uniform float u_maxzoom;
uniform sampler2D u_fadetexture;

// v_max_zoom is a collision-box-specific value that controls when line-following
// collision boxes are used.
varying float v_max_zoom;
varying float v_placement_zoom;
varying float v_perspective_zoom_adjust;
varying vec2 v_fade_tex;

void main() {

    float alpha = 0.5;

    // Green = no collisions, label is showing
    gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0) * alpha;

    // Red = collision, label hidden
    if (texture2D(u_fadetexture, v_fade_tex).a < 1.0) {
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * alpha;
    }

    // Faded black = this collision box is not used at this zoom (for curved labels)
    if (u_zoom >= v_max_zoom + v_perspective_zoom_adjust) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0) * alpha * 0.25;
    }

    // Faded blue = the placement scale for this label is beyond the CollisionTile
    // max scale, so it's impossible for this label to show without collision detection
    // being run again (the label's glyphs haven't even been added to the symbol bucket)
    if (v_placement_zoom >= u_maxzoom) {
        gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0) * alpha * 0.2;
    }
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
