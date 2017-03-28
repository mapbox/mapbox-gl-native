// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/fill.hpp>

namespace mbgl {
namespace shaders {

const char* fill::name = "fill";
const char* fill::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;

uniform mat4 u_matrix;

uniform lowp float a_color_t;
attribute highp vec4 a_color;
varying highp vec4 color;
uniform lowp float a_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;

void main() {
    color = unpack_mix_vec4(a_color, a_color_t);
    opacity = unpack_mix_vec2(a_opacity, a_opacity_t);

    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

)MBGL_SHADER";
const char* fill::fragmentSource = R"MBGL_SHADER(
varying highp vec4 color;
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
