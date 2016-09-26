#include <mbgl/shader/line_vertex.hpp>
#include <mbgl/shader/shader.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

void LineVertex::bind(const int8_t* offset) {
    static_assert(sizeof(LineVertex) == 8, "expected LineVertex size");

    MBGL_BIND_VERTEX_ATTRIBUTE(LineVertex, a_pos, offset);
    MBGL_BIND_VERTEX_ATTRIBUTE(LineVertex, a_data, offset);
}

} // namespace mbgl
