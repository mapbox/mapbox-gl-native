#include <mbgl/shader/plain_vertex.hpp>
#include <mbgl/shader/shader.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

void PlainVertex::bind(const int8_t* offset) {
    static_assert(sizeof(PlainVertex) == 4, "expected PlainVertex size");

    MBGL_BIND_VERTEX_ATTRIBUTE(PlainVertex, a_pos, offset);
}

} // namespace mbgl
