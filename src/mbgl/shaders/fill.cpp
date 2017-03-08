// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/fill.hpp>

namespace mbgl {
namespace shaders {

const char* fill::name = "fill";
const char* fill::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;

uniform mat4 u_matrix;

uniform lowp float a_color_t;
attribute lowp vec4 a_color_min;
attribute lowp vec4 a_color_max;
varying lowp vec4 color;
uniform lowp float a_opacity_t;
attribute lowp float a_opacity_min;
attribute lowp float a_opacity_max;
varying lowp float opacity;

void main() {
    color = mix(a_color_min, a_color_max, a_color_t);
    opacity = mix(a_opacity_min, a_opacity_max, a_opacity_t);

    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

)MBGL_SHADER";
const char* fill::fragmentSource = R"MBGL_SHADER(
varying lowp vec4 color;
varying lowp float opacity;

void main() {
    
    

    gl_FragColor = color * opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
