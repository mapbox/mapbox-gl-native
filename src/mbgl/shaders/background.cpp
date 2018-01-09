// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/background.hpp>

namespace mbgl {
namespace shaders {

const char* background::name = "background";
const char* background::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

)MBGL_SHADER";
const char* background::fragmentSource = R"MBGL_SHADER(
uniform vec4 u_color;
uniform float u_opacity;

void main() {
    gl_FragColor = u_color * u_opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
