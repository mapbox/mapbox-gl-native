#include <mbgl/shader/collision_box_vertex.hpp>
#include <mbgl/shader/shader.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

void CollisionBoxVertex::bind(const int8_t* offset) {
    static_assert(sizeof(CollisionBoxVertex) == 10, "expected CollisionBoxVertex size");

    MBGL_BIND_VERTEX_ATTRIBUTE(CollisionBoxVertex, a_pos, offset);
    MBGL_BIND_VERTEX_ATTRIBUTE(CollisionBoxVertex, a_extrude, offset);
    MBGL_BIND_VERTEX_ATTRIBUTE(CollisionBoxVertex, a_data, offset);
}

} // namespace mbgl
