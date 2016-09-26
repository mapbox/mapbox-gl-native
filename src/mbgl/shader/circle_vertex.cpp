#include <mbgl/shader/circle_vertex.hpp>
#include <mbgl/shader/shader.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

void CircleVertex::bind(const int8_t* offset) {
    static_assert(sizeof(CircleVertex) == 4, "expected CircleVertex size");

    MBGL_BIND_VERTEX_ATTRIBUTE(CircleVertex, a_pos, offset);
}

} // namespace mbgl
