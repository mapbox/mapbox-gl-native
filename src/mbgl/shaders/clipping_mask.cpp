// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/clipping_mask.hpp>

namespace mbgl {
namespace shaders {

const char* clipping_mask::name = "clipping_mask";
const char* clipping_mask::vertexSource = R"MBGL_SHADER(
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

)MBGL_SHADER";
const char* clipping_mask::fragmentSource = R"MBGL_SHADER(
void main() {
    gl_FragColor = vec4(1.0);
}

)MBGL_SHADER";

} // namespace shaders
} // namespace mbgl
