// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/background.hpp>
#include <mbgl/shaders/source.hpp>

namespace mbgl {
namespace shaders {

const char* background::name = "background";
const char* background::vertexSource = source() + 2739;
const char* background::fragmentSource = source() + 2851;

// Uncompressed source of background.vertex.glsl:
/*
attribute vec2 a_pos;

uniform mat4 u_matrix;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

*/

// Uncompressed source of background.fragment.glsl:
/*
uniform vec4 u_color;
uniform float u_opacity;

void main() {
    gl_FragColor = u_color * u_opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/

} // namespace shaders
} // namespace mbgl
