// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/heatmap_texture.hpp>

namespace mbgl {
namespace shaders {

const char* heatmap_texture::name = "heatmap_texture";
const char* heatmap_texture::vertexSource = R"MBGL_SHADER(
uniform mat4 u_matrix;
uniform vec2 u_world;
attribute vec2 a_pos;
varying vec2 v_pos;

void main() {
    gl_Position = u_matrix * vec4(a_pos * u_world, 0, 1);

    v_pos.x = a_pos.x;
    v_pos.y = 1.0 - a_pos.y;
}

)MBGL_SHADER";
const char* heatmap_texture::fragmentSource = R"MBGL_SHADER(
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

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
