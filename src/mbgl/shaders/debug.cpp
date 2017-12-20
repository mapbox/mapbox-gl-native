// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/debug.hpp>

namespace mbgl {
namespace shaders {

const char* debug::name = "debug";
const char* debug::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

)MBGL_SHADER";
const char* debug::fragmentSource = R"MBGL_SHADER(
uniform highp vec4 u_color;

void main() {
    gl_FragColor = u_color;
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
