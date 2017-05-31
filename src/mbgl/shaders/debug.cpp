// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/debug.hpp>

namespace mbgl {
namespace shaders {

const char* debug::name = "debug";
const char* debug::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    // We are using Int16 for texture position coordinates to give us enough precision for
    // fractional coordinates. We use 8192 to scale the texture coordinates in the buffer
    // as an arbitrarily high number to preserve adequate precision when rendering.
    // This is also the same value as the EXTENT we are using for our tile buffer pos coordinates,
    // so math for modifying either is consistent.
    gl_Position = u_matrix * vec4(a_pos, step(8192.0, a_pos.x), 1);
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
