// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/clipping_mask.hpp>
#include <mbgl/shaders/source.hpp>

namespace mbgl {
namespace shaders {

const char* clipping_mask::name = "clipping_mask";
const char* clipping_mask::vertexSource = source() + 10633;
const char* clipping_mask::fragmentSource = source() + 10745;

// Uncompressed source of clipping_mask.vertex.glsl:
/*
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

*/

// Uncompressed source of clipping_mask.fragment.glsl:
/*
void main() {
    gl_FragColor = vec4(1.0);
}

*/

} // namespace shaders
} // namespace mbgl
