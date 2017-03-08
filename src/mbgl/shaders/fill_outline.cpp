// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/fill_outline.hpp>

namespace mbgl {
namespace shaders {

const char* fill_outline::name = "fill_outline";
const char* fill_outline::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;

uniform mat4 u_matrix;
uniform vec2 u_world;

varying vec2 v_pos;

uniform lowp float a_outline_color_t;
attribute lowp vec4 a_outline_color_min;
attribute lowp vec4 a_outline_color_max;
varying lowp vec4 outline_color;
uniform lowp float a_opacity_t;
attribute lowp float a_opacity_min;
attribute lowp float a_opacity_max;
varying lowp float opacity;

void main() {
    outline_color = mix(a_outline_color_min, a_outline_color_max, a_outline_color_t);
    opacity = mix(a_opacity_min, a_opacity_max, a_opacity_t);

    gl_Position = u_matrix * vec4(a_pos, 0, 1);
    v_pos = (gl_Position.xy / gl_Position.w + 1.0) / 2.0 * u_world;
}

)MBGL_SHADER";
const char* fill_outline::fragmentSource = R"MBGL_SHADER(
varying lowp vec4 outline_color;
varying lowp float opacity;

varying vec2 v_pos;

void main() {
    
    

    float dist = length(v_pos - gl_FragCoord.xy);
    float alpha = smoothstep(1.0, 0.0, dist);
    gl_FragColor = outline_color * (alpha * opacity);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
