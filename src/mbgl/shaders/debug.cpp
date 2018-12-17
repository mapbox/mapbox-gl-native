// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/debug.hpp>
#include <mbgl/shaders/source.hpp>

namespace mbgl {
namespace shaders {

const char* debug::name = "debug";
const char* debug::vertexSource = source() + 12417;
const char* debug::fragmentSource = source() + 12513;

// Uncompressed source of debug.vertex.glsl:
/*
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

*/

// Uncompressed source of debug.fragment.glsl:
/*
uniform highp vec4 u_color;

void main() {
    gl_FragColor = u_color;
}

*/

} // namespace shaders
} // namespace mbgl
