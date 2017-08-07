// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/raster.hpp>

namespace mbgl {
namespace shaders {

const char* raster::name = "raster";
const char* raster::vertexSource = R"MBGL_SHADER(
uniform mat4 u_matrix;
uniform vec2 u_tl_parent;
uniform float u_scale_parent;
uniform float u_buffer_scale;

attribute vec2 a_pos;
attribute vec2 a_texture_pos;

varying vec2 v_pos0;
varying vec2 v_pos1;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
    // We are using Int16 for texture position coordinates to give us enough precision for
    // fractional coordinates. We use 8192 to scale the texture coordinates in the buffer
    // as an arbitrarily high number to preserve adequate precision when rendering.
    // This is also the same value as the EXTENT we are using for our tile buffer pos coordinates,
    // so math for modifying either is consistent.
    v_pos0 = (((a_texture_pos / 8192.0) - 0.5) / u_buffer_scale ) + 0.5;
    v_pos1 = (v_pos0 * u_scale_parent) + u_tl_parent;
}

)MBGL_SHADER";
const char* raster::fragmentSource = R"MBGL_SHADER(
uniform float u_fade_t;
uniform float u_opacity;
uniform sampler2D u_image0;
uniform sampler2D u_image1;
varying vec2 v_pos0;
varying vec2 v_pos1;

uniform float u_brightness_low;
uniform float u_brightness_high;

uniform float u_saturation_factor;
uniform float u_contrast_factor;
uniform vec3 u_spin_weights;

void main() {

    // read and cross-fade colors from the main and parent tiles
    vec4 color0 = texture2D(u_image0, v_pos0);
    vec4 color1 = texture2D(u_image1, v_pos1);
    if (color0.a > 0.0) {
        color0.rgb = color0.rgb / color0.a;
    }
    if (color1.a > 0.0) {
        color1.rgb = color1.rgb / color1.a;
    }
    vec4 color = mix(color0, color1, u_fade_t);
    color.a *= u_opacity;
    vec3 rgb = color.rgb;

    // spin
    rgb = vec3(
        dot(rgb, u_spin_weights.xyz),
        dot(rgb, u_spin_weights.zxy),
        dot(rgb, u_spin_weights.yzx));

    // saturation
    float average = (color.r + color.g + color.b) / 3.0;
    rgb += (average - rgb) * u_saturation_factor;

    // contrast
    rgb = (rgb - 0.5) * u_contrast_factor + 0.5;

    // brightness
    vec3 u_high_vec = vec3(u_brightness_low, u_brightness_low, u_brightness_low);
    vec3 u_low_vec = vec3(u_brightness_high, u_brightness_high, u_brightness_high);

    gl_FragColor = vec4(mix(u_high_vec, u_low_vec, rgb) * color.a, color.a);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
